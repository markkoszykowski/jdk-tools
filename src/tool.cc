#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


constexpr std::string_view ANSI_CSI{"\033["};
constexpr char ANSI_SEPERATOR{';'};
constexpr char ANSI_SGR{'m'};
constexpr std::string_view ANSI_RESET{"0"};
constexpr std::string_view ANSI_BOLD{"1"};
constexpr std::string_view ANSI_RED{"31"};

constexpr std::string JAVA_HOME{"JAVA_HOME"};
constexpr std::string_view BIN{"bin"};


template<typename... Args>
void error(Args&&... args) {
	std::cerr << ::ANSI_CSI << ::ANSI_RED << ::ANSI_SGR;
	((std::cerr << args << ' '), ...);
	std::cerr << ::ANSI_CSI << ::ANSI_RESET << ::ANSI_SGR;
	std::cerr << '\n';
}

template<typename... Args>
void java_home_error(Args&&... args) {
	std::cerr << ::ANSI_CSI << ::ANSI_RED << ::ANSI_SGR << '\'';
	std::cerr << ::ANSI_CSI << ::ANSI_BOLD << ::ANSI_SEPERATOR << ::ANSI_RED << ::ANSI_SGR << ::JAVA_HOME;
	std::cerr << ::ANSI_CSI << ::ANSI_RED << ::ANSI_SGR << '\'';
	std::cerr << ' ';
	::error(args...);
}

inline std::optional<std::string_view> env(const std::string& key) {
	const char* value{std::getenv(key.c_str())};
	return value == nullptr ?
			std::optional<std::string_view>{std::nullopt} :
			std::optional<std::string_view>{std::string_view{value}};
}

inline std::filesystem::path tool(const std::filesystem::path& java_home, const char* const* argv) {
	const std::filesystem::path tool{argv[0]};
	return java_home / BIN / tool.filename();
}

inline void exec(const std::filesystem::path& tool, const int argc, const char* const* argv) {
	std::vector<const char*> args{};
	args.reserve(static_cast<std::size_t>(argc) + 1U);

	args.push_back(tool.c_str());
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[static_cast<std::size_t>(i)]);
	}
	args.push_back(nullptr);

	// https://stackoverflow.com/questions/190184/execv-and-const-ness
	if (::execv(tool.c_str(), const_cast<char* const*>(args.data())) == -1) {
		::java_home_error("could not be executed", ':', std::strerror(errno));
	} else {
		::error("Unknown error occurred");
	}
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

	::exec(tool, argc, argv);

	return -4;
}
