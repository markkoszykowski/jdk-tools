#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>


constexpr std::string_view ANSI_ESCAPE{"\033["};
constexpr char ANSI_SEPERATOR{';'};
constexpr char ANSI_SGR{'m'};
constexpr std::string_view RESET{"0"};
constexpr std::string_view BOLD{"1"};
constexpr std::string_view RED{"31"};

constexpr std::string JAVA_HOME{"JAVA_HOME"};


inline std::ostream& java_home_error(const std::string_view message) {
	return std::cerr << ANSI_ESCAPE << RED << ANSI_SGR << '\'' <<
		ANSI_ESCAPE << BOLD << ANSI_SEPERATOR << RED << ANSI_SGR << JAVA_HOME <<
		ANSI_ESCAPE << RED << ANSI_SGR << "\' " <<
		message << ANSI_ESCAPE << RESET << ANSI_SGR << '\n';
}

inline std::optional<std::string_view> env(const std::string& key) {
	const char* value{std::getenv(key.c_str())};
	return value == nullptr ? std::optional<std::string_view>{} : std::optional<std::string_view>{value};
}

inline std::filesystem::path tool(const char** argv, const std::filesystem::path java_home) {
	const std::filesystem::path tool{argv[0]};
	return java_home / tool.filename();
}

inline std::string command(const int argc, const char** argv, const std::filesystem::path tool) {
	std::string command{tool};

	for (int i = 1; i < argc; ++i) {
		command.push_back(' ');
		command.append(argv[static_cast<std::size_t>(i)]);
	}

	return command;
}

int main(const int argc, const char** argv) {
	const std::optional<std::string_view> java_home{::env(JAVA_HOME)};
	if (!java_home.has_value()) {
		::java_home_error("is not defined");
		return -1;
	}

	const std::filesystem::path jdk{java_home.value()};
	if (!std::filesystem::exists(jdk)) {
		::java_home_error("does not exist");
		return -2;
	}

	const std::filesystem::path tool{::tool(argv, jdk)};
	if (!std::filesystem::exists(tool)) {
		::java_home_error("is not a valid JDK");
		return -3;
	}

	return std::system(::command(argc, argv, tool).c_str());
}
