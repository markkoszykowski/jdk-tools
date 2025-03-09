use std::path::Path;

macro_rules! cargo_println {
    ( $( $arg:tt )* ) => {
        println!("cargo::warning={}", format!( $( $arg )* ))
    }
}

macro_rules! cargo_eprintln {
    ( $( $arg:tt )* ) => {
        println!("cargo::error={}", format!( $( $arg )* ))
    }
}

fn artifact_dir() -> Option<Box<str>> {
    let out_dir: String = std::env::var("OUT_DIR").ok()?;
    let artifact_dir: &Path = Path::new(&out_dir).parent()?.parent()?.parent()?;
    if artifact_dir.exists() {
        Some(artifact_dir.to_str()?.to_string().into_boxed_str())
    } else {
        None
    }
}

fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    match artifact_dir() {
        Some(artifact_dir) => {
            cargo_println!("Please add {} to your PATH", artifact_dir);
            cargo_println!("i.e.");
            cargo_println!("  export PATH=\"${{PATH}}:{}\"", artifact_dir);
            cargo_println!("");
        }
        None => cargo_eprintln!("Unable to determine artifact-dir"),
    }
}
