#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <vector>


constexpr std::string_view ANSI_ESCAPE{"\033["};
constexpr char ANSI_SEPERATOR{';'};
constexpr char ANSI_SGR{'m'};
constexpr std::string_view RESET{"0"};
constexpr std::string_view BOLD{"1"};
constexpr std::string_view RED{"31"};

constexpr std::string JAVA_HOME{"JAVA_HOME"};


template<typename... Args>
inline void error(Args&& ... args) {
	std::cerr << ::ANSI_ESCAPE << ::RED << ::ANSI_SGR;
	((std::cerr << args << ' '), ...);
	std::cerr << ::ANSI_ESCAPE << ::RESET << ::ANSI_SGR;
	std::cerr << '\n';
}

template<typename... Args>
inline void java_home_error(Args&& ... args) {
	std::cerr << ::ANSI_ESCAPE << ::RED << ::ANSI_SGR << '\'';
	std::cerr << ::ANSI_ESCAPE << ::BOLD << ::ANSI_SEPERATOR << ::RED << ::ANSI_SGR << ::JAVA_HOME;
	std::cerr << ::ANSI_ESCAPE << ::RED << ::ANSI_SGR << '\'';
	std::cerr << ' ';
	::error(args...);
}

inline std::optional<std::string_view> env(const std::string& key) {
	const char* value{std::getenv(key.c_str())};
	return value == nullptr ? std::optional<std::string_view>{std::nullopt} : std::optional<std::string_view>{value};
}

inline std::filesystem::path tool(const std::filesystem::path& java_home, const char* const* argv) {
	const std::filesystem::path tool{argv[0]};
	return java_home / tool.filename();
}

inline int exec(const std::filesystem::path& tool, const int argc, const char* const* argv) {
	std::vector<const char*> args{};
	args.reserve(static_cast<std::size_t>(argc) + 1);

	args.push_back(tool.c_str());
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[static_cast<std::size_t>(i)]);
	}
	args.push_back(nullptr);

	// https://stackoverflow.com/questions/190184/execv-and-const-ness
	if (::execv(tool.c_str(), const_cast<char* const*>(args.data())) != -1) {
		::error("Unknown error occurred");
		std::exit(1);
	}
	return errno;
}

int main(const int argc, const char* const* argv) {
	const std::optional<std::string_view> java_home{::env(::JAVA_HOME)};
	if (!java_home.has_value()) {
		::java_home_error("is not defined");
		return -1;
	}

	const std::filesystem::path jdk{java_home.value()};
	if (!std::filesystem::exists(jdk)) {
		::java_home_error("does not exist");
		return -2;
	}

	const std::filesystem::path tool{::tool(jdk, argv)};
	if (!std::filesystem::exists(tool)) {
		::java_home_error("is not a valid JDK");
		return -3;
	}

	const int error{::exec(tool, argc, argv)};
	::java_home_error("could not be executed", ':', std::strerror(error));

	return -4;
}
