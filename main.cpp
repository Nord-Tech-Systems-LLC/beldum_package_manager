#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

bool show_warning() {
    // Display the warning message
    std::cout << "WARNING: This operation may have consequences.\n";
    std::cout << "Do you want to proceed? (y/n): ";

    // Get user input
    char response;
    std::cin >> response;

    // flush the input buffer to avoid issues with getline
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // verification
    std::cout << "You entered:" << response << "\n";
    // Check user response
    if (response == 'y' || response == 'Y') {
        std::cout << "Proceeding...\n";
        return true;
    } else {
        std::cout << "Operation canceled.\n";
        return false;
    }
}

// enum to represent possible command line options
enum class PossibleOptions {
    NONE,
    INSTALL,
    CLEAN,
    HELP,
    VERSION,
    INPUT_FILE,
    OUTPUT_FILE
};

// function to parse command line arguments and assign them to the enum
PossibleOptions parse_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        if (std::strcmp(arg, "--help") == 0) {
            return PossibleOptions::HELP;
        } else if (std::strcmp(arg, "--install") == 0) {
            return PossibleOptions::INSTALL;
        } else if (std::strcmp(arg, "--clean") == 0) {
            // if warning returns true
            if (show_warning()) {
                return PossibleOptions::CLEAN;
            } else {
                return PossibleOptions::NONE;
            }
        } else if (std::strcmp(arg, "--version") == 0) {
            return PossibleOptions::VERSION;
        } else if (std::strcmp(arg, "-i") == 0 || std::strcmp(arg, "--input") == 0) {
            // assuming that the next argument is the input file name
            return (i + 1 < argc) ? PossibleOptions::INPUT_FILE : PossibleOptions::NONE;
        } else if (std::strcmp(arg, "-o") == 0 || std::strcmp(arg, "--output") == 0) {
            // assuming that the next argument is the output file name
            return (i + 1 < argc) ? PossibleOptions::OUTPUT_FILE : PossibleOptions::NONE;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            return PossibleOptions::NONE;
        }
    }

    // No valid options found
    return PossibleOptions::NONE;
}

void print_options_available(std::vector<const char*>& possible_shell_arguments) {
    for (int i = 0; i < possible_shell_arguments.size(); i++) {
        // printing every element of input
        std::cout << possible_shell_arguments[i] << std::endl;
    }
}

void check_passed_shell_arguments(PossibleOptions options) {
    // read shell arguments passed
    int return_code;
    std::string command;
    std::string repository_URL;
    std::string repo_name;

    switch (options) {
        // help actions
        case PossibleOptions::HELP:
            std::cout << "Help printed" << std::endl;
            break;

        // install actions
        case PossibleOptions::INSTALL:
            // Replace the repository URL with the one you want to clone
            repo_name = "cpp_webserver";
            repository_URL = "git@github.com:Nord-Tech-Systems-LLC/cpp_webserver.git libs/" + std::string(repo_name);

            // using bash to clone the repo
            command = "git clone " + std::string(repository_URL);
            return_code = system(command.c_str());

            break;

        // clean actions
        case PossibleOptions::CLEAN:
            // using bash to clean the library folder
            command = "sudo rm -r libs/*";
            return_code = system(command.c_str());
            break;
    }
}

int main(int argc, char* argv[]) {
    PossibleOptions option = parse_arguments(argc, argv);
    check_passed_shell_arguments(option);

    return 0;
}