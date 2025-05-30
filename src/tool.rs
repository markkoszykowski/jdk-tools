use std::borrow::Cow;
use std::borrow::Cow::{Borrowed, Owned};
use std::ffi::{c_char, CStr, CString, OsStr, OsString};
use std::io::Error;
use std::path::Path;
use std::process::ExitCode;

const ANSI_CSI: &str = "\x1b[";
const ANSI_SEPARATOR: char = ';';
const ANSI_SGR: char = 'm';
const ANSI_RESET: &str = "0";
const ANSI_BOLD: &str = "1";
const ANSI_RED: &str = "31";

const JAVA_HOME: &str = "JAVA_HOME";
const BIN: &str = "bin";

macro_rules! error {
    ( $( $arg:expr ),+ ) => {
        {
            eprint!("{ANSI_CSI}{ANSI_RED}{ANSI_SGR}");
            let mut _first: bool = true;
            $(
                if !_first {
                    eprint!(" ");
                }
                eprint!("{}", $arg);
                _first = false;
            )*
            eprint!("{ANSI_CSI}{ANSI_RESET}{ANSI_SGR}");
            eprintln!();
        }
    };
}

macro_rules! java_home_error {
    ( $( $arg:expr ),+ ) => {
        {
            eprint!("{ANSI_CSI}{ANSI_RED}{ANSI_SGR}'");
            eprint!("{ANSI_CSI}{ANSI_BOLD}{ANSI_SEPARATOR}{ANSI_RED}{ANSI_SGR}{JAVA_HOME}");
            eprint!("{ANSI_CSI}{ANSI_RED}{ANSI_SGR}'");
            eprint!(" ");
            error!( $( $arg ),+ );
        }
    };
}

macro_rules! c_string {
    ( $os_str:expr ) => {{
        match CString::new($os_str.as_encoded_bytes()) {
            Ok(os_str) => os_str,
            Err(error) => {
                java_home_error!("could not be executed", ":", error);
                return;
            }
        }
    }};
}

#[inline]
fn tool<T>(java_home: &Path, args: &[T]) -> Option<OsString>
where
    T: AsRef<OsStr>,
{
    let tool: &Path = Path::new(args.first()?);
    Some(java_home.join(BIN).join(tool.file_name()?).into_os_string())
}

#[inline]
fn execv<T>(tool: &OsStr, args: &[T])
where
    T: AsRef<OsStr>,
{
    let tool: Box<CStr> = c_string!(tool).into_boxed_c_str();
    let args: Box<[Cow<CStr>]> = {
        let mut argv: Vec<Cow<CStr>> = Vec::with_capacity(args.len());
        argv.push(Borrowed(&tool));
        for arg in &args[1..] {
            argv.push(Owned(c_string!(arg.as_ref())));
        }
        argv.into_boxed_slice()
    };

    let prog: *const c_char = tool.as_ptr();
    let argv: Box<[*const c_char]> = args
        .iter()
        .map(Cow::as_ref)
        .map(CStr::as_ptr)
        .chain([std::ptr::null()])
        .collect();

    if unsafe { libc::execv(prog, argv.as_ptr()) } == -1 {
        java_home_error!("could not be executed", ":", Error::last_os_error());
    } else {
        error!("Unknown error occurred");
    }
}

fn main() -> ExitCode {
    let args: Box<[Box<OsStr>]> = std::env::args_os()
        .map(OsString::into_boxed_os_str)
        .collect();

    let java_home: Box<OsStr> = match std::env::var_os(JAVA_HOME) {
        Some(var) => var.into_boxed_os_str(),
        None => {
            java_home_error!("is not defined");
            return ExitCode::FAILURE;
        }
    };

    let jdk: &Path = Path::new(&java_home);
    if !jdk.exists() {
        java_home_error!("does not exist");
        return ExitCode::FAILURE;
    }

    let tool: Box<OsStr> = match tool(&jdk, &args) {
        Some(tool) => tool.into_boxed_os_str(),
        None => {
            java_home_error!("is not a valid JDK");
            return ExitCode::FAILURE;
        }
    };

    execv(&tool, &args);

    ExitCode::FAILURE
}
