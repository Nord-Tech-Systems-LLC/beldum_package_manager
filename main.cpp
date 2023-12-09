#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

// enum to represent possible command line options
enum class Option {
    NONE,
    INSTALL,
    HELP,
    VERSION,
    INPUT_FILE,
    OUTPUT_FILE
};

// function to parse command line arguments and assign them to the enum
Option parse_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        if (std::strcmp(arg, "--help") == 0) {
            return Option::HELP;
        } else if (std::strcmp(arg, "--install") == 0) {
            return Option::INSTALL;
        } else if (std::strcmp(arg, "--version") == 0) {
            return Option::VERSION;
        } else if (std::strcmp(arg, "-i") == 0 || std::strcmp(arg, "--input") == 0) {
            // Assuming that the next argument is the input file name
            return (i + 1 < argc) ? Option::INPUT_FILE : Option::NONE;
        } else if (std::strcmp(arg, "-o") == 0 || std::strcmp(arg, "--output") == 0) {
            // Assuming that the next argument is the output file name
            return (i + 1 < argc) ? Option::OUTPUT_FILE : Option::NONE;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            return Option::NONE;
        }
    }

    // No valid options found
    return Option::NONE;
}

void print_options_available(std::vector<const char*> &possible_shell_arguments) {
    for (int i = 0; i < possible_shell_arguments.size(); i++) {
        // printing every element of input
        std::cout << possible_shell_arguments[i] << std::endl;
    }
}

bool check_passed_shell_arguments(Option options) {
    // read shell arguments passed
    switch (options) {
        case Option::HELP:
            std::cout << "Help printed" << std::endl;
            break;
        case Option::INSTALL:
            // Replace the repository URL with the one you want to clone
            const char* repository_URL = "git@github.com:Nord-Tech-Systems-LLC/cpp_webserver.git libs";
            std::string command = "git clone " + std::string(repository_URL);
            int return_code = system(command.c_str());
            // std::cout << "Install printed" << std::endl;
            break;
    }

    return true;
}

int main(int argc, char *argv[]) {
    Option option = parse_arguments(argc, argv);
    std::vector<std::string> packages;

    // print_options_available(possible_shell_arguments);
    check_passed_shell_arguments(option);

    // std::cout << "testing" << std::endl;

    return 0;
}