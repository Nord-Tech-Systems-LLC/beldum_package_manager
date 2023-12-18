#include "headerfiles/package_manager.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

#include "../cpp_libs/json/single_include/nlohmann/json.hpp"

Package individual_package;
namespace fs = std::filesystem;

void PackageManager::check_passed_shell_arguments(PossibleOptions options) {
    // read shell arguments passed
    int return_code;
    std::string command;
    std::string repository_URL;
    std::string repo_name;

    // gets list of packages
    std::ifstream packages_file("package.json");
    std::ifstream installed_packages("installed_packages.json");

    // instantiate json object
    using json = nlohmann::json;
    json data;
    json installed_data;

    std::string requested_package = individual_package.name;
    std::string testing = std::string(fs::current_path());
    std::string result_string;
    switch (options) {
        /**
         * HELP ACTIONS
         */
        case PossibleOptions::HELP:
            print_help();
            // std::cout << "Help printed" << std::endl;
            break;

        /**
         * INSTALL ACTIONS
         */
        case PossibleOptions::INSTALL:
            data = json::parse(packages_file);

            std::cout << data.dump(4) << std::endl;

            repo_name = data["packages"][requested_package]["repo_name"];
            repository_URL = data["packages"][requested_package]["git_link"];

            // strips quotes from directory
            for (char c : testing) {
                if (c != '\"') {
                    result_string += c;
                }
            }
            // unsure if this is needed yet
            // std::cout.flush();

            std::cout << result_string << std::endl;
            // using bash to clone the repo
            command = "cd " + result_string + " && git clone " + std::string(repository_URL) + " cpp_libs/" + std::string(repo_name);
            std::cout << "Command: " << command << std::endl;
            return_code = system(command.c_str());

            // updating installed package manager
            installed_data = json::parse(installed_packages);
            installed_data["packages"] = {
                {"cpp_dependency_management", {
                                                  {"git_link", "git@github.com:Nord-Tech-Systems-LLC/cpp_dependency_management.git"},
                                                  {"repo_name", "cpp_dependency_management"},
                                                  {"version", "v0.0.0"},
                                                  {"package_rsa", "ssh-rsa blah"},
                                              }}};

            // TODO: need to output to json file
            std::cout << installed_data.dump(4) << std::endl;

            break;

            /**
             * LIST PACKAGES
             */
        case PossibleOptions::LIST_PACKAGES:

            data = json::parse(packages_file);

            // prints header
            std::cout << std::setw(40) << std::left << "\n\n\nPACKAGE:";
            std::cout << std::setw(20) << std::right << "VERSION:" << std::endl;
            std::cout << std::setw(80) << std::left << "------------------------------------------------------------" << std::endl;
            for (auto test : data["packages"]) {
                // prints all packages
                std::cout << std::setw(40) << std::left << std::string(test["repo_name"]);
                std::cout << std::setw(20) << std::right << std::string(test["version"]) << std::endl;
            }
            // space at bottom
            std::cout << "\n\n\n"
                      << std::endl;
            break;

        /**
         * CLEAN ACTIONS
         */
        case PossibleOptions::CLEAN:
            // using bash to clean the library folder
            command = "sudo rm -r cpp_libs";
            return_code = system(command.c_str());
            break;
    }
}

// function to parse command line arguments and assign them to the enum
PossibleOptions PackageManager::parse_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        if (std::strcmp(arg, "--help") == 0) {
            return PossibleOptions::HELP;
        } else if (std::strcmp(arg, "--install") == 0) {
            // Check if the next argument exists
            if (i + 1 < argc) {
                // Assuming the next argument is the package name
                individual_package.name = argv[i + 1];
                return PossibleOptions::INSTALL;
            } else {
                std::cerr << "Error: Missing package name after '--install'." << std::endl;
                return PossibleOptions::NONE;
            }
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
        } else if (std::strcmp(arg, "--list") == 0) {
            return PossibleOptions::LIST_PACKAGES;

        } else {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            return PossibleOptions::NONE;
        }
    }

    // No valid options found
    return PossibleOptions::NONE;
}

bool PackageManager::show_warning() {
    // TODO: on presssing no, command still runs -- need to fix
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

// TODO: need to remove
void PackageManager::print_table(const std::map<std::string, std::string>& package) {
    // Determine column widths
    std::vector<size_t> column_widths = {0, 0, 0};

    // Print the contents of the map using iterators
    for (auto it = package.begin(); it != package.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

void create_row_help_menu(const std::string& command_flag, const std::string& description) {
    std::cout << std::setw(20) << std::left << command_flag;
    std::cout << std::setw(60) << std::right << description << std::endl;
}

void PackageManager::print_help() {
    std::cout << "\n\n\n"
              << std::endl;
    create_row_help_menu("COMMAND:", "DESCRIPTION:");
    std::cout << std::setw(80) << std::left << "--------------------------------------------------------------------------------" << std::endl;
    create_row_help_menu("--list", "to list installed packages");
    create_row_help_menu("--install", "to install packages");
    create_row_help_menu("--uninstall", "to uninstall packages");  // TODO: not created yet
    create_row_help_menu("--help", "to show commands");
    create_row_help_menu("--clean", "to remove contents from cpp_libs folder");
    std::cout << "\n\n\n"
              << std::endl;
}