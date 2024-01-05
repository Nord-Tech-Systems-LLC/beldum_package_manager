#include "headerfiles/package_manager.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

// #define JSON_DEPENDENCY_EXIST # for vscode prettier, comment out when not coding

#ifdef JSON_DEPENDENCY_EXIST
#include "json/single_include/nlohmann/json.hpp"

Package individual_package;
namespace fs = std::filesystem;

inline bool PackageManager::file_exists(const std::string& name) {
    std::ifstream file(name.c_str());
    return file.good();
}

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
         * INIT ACTIONS
         */
        case PossibleOptions::INIT:
            // creates packages folder if it doesn't exist
            std::cout << "\n";
            if (file_exists("installed_packages.json") && file_exists("package.json")) {
                std::cout << "Package.json and installed_packages.json already exist.\n"
                          << "Try installing an example package with --install example_package\n"
                          << std::endl;
                break;
            } else {
                if (!file_exists("installed_packages.json")) {
                    std::cout << "Creating installed_packages.json" << std::endl;
                    std::ofstream output("installed_packages.json");
                    installed_data["packages"] = {};
                    output << installed_data;
                }
                if (!file_exists("package.json")) {
                    std::cout << "Creating package.json" << std::endl;
                    std::ofstream output("package.json");
                    data["packages"] = {
                        {"example_package", {
                                                {"git_link", "git@github.com:Nord-Tech-Systems-LLC/cpp_webserver.git"},
                                                {"repo_name", "example_package"},
                                                {"version", "v0.0.0"},
                                                {"package_rsa", "ssh-rsa blah"},
                                            }}};

                    output << data;
                }
            }

            std::cout << "\n";
            break;

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
            // parse package.json and installed_packages.json
            data = json::parse(packages_file);
            installed_data = json::parse(installed_packages);

            // checks if package is already installed
            if (installed_data["packages"].contains(requested_package)) {
                std::cout << "\nThe package \"" << requested_package << "\" is already installed...\n"
                          << std::endl;
                break;
            };

            // checks if package exists in package.json
            if (data["packages"].contains(requested_package)) {
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
                std::ofstream output("installed_packages.json");
                installed_data["packages"][repo_name] = {
                    {"git_link", repository_URL},
                    {"repo_name", repo_name},
                    {"version", "v0.0.0"},
                    {"package_rsa", "ssh-rsa blah"},
                };

                output << installed_data;
            } else {
                std::cout << "\nPackage \"" << requested_package << "\" does not exist. Please enter the correct package name.\n"
                          << std::endl;
            }

            break;

            /**
             * LIST PACKAGES
             */
        case PossibleOptions::LIST_PACKAGES:

            if (file_exists("installed_packages.json")) {
                installed_data = json::parse(installed_packages);

                // prints header
                std::cout << std::setw(40) << std::left << "\n\n\nPACKAGE:";
                std::cout << std::setw(20) << std::right << "VERSION:" << std::endl;
                std::cout << std::setw(80) << std::left << "------------------------------------------------------------" << std::endl;
                for (auto test : installed_data["packages"]) {
                    // prints all packages
                    std::cout << std::setw(40) << std::left << std::string(test["repo_name"]);
                    std::cout << std::setw(20) << std::right << std::string(test["version"]) << std::endl;
                }
                // space at bottom
                std::cout << "\n\n"
                          << std::endl;
            } else {
                std::cerr << "\nThe installed_packages.json does not exist, please run --init" << std::endl;
            }
            break;

        /**
         * CLEAN ACTIONS
         */
        case PossibleOptions::CLEAN:
            // using bash to clean the library folder

            // TODO: need to use some variation of this: find cpp_libs ! -name 'json' -type d -exec rm -rf {} +
            command = "sudo rm -r cpp_libs";
            std::ofstream output("installed_packages.json");
            installed_data["packages"] = {};
            output << installed_data;
            return_code = system(command.c_str());
            std::cout << "\nSuccessfully cleaned project directory.\n"
                      << std::endl;
            break;
    }
}

// function to parse command line arguments and assign them to the enum
PossibleOptions PackageManager::parse_arguments(int argc, char* argv[]) {
    std::map<std::string, PossibleOptions> command_map = {
        {"--help", PossibleOptions::HELP},
        {"--init", PossibleOptions::INIT},
        {"--install", PossibleOptions::INSTALL},
        {"--clean", PossibleOptions::CLEAN},
        {"--version", PossibleOptions::VERSION},
        {"--list", PossibleOptions::LIST_PACKAGES}};

    // traverse passed command arguments
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        // if command isn't found, else return PossibleOptions
        if (command_map.find(arg) == command_map.end()) {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            return PossibleOptions::NONE;
        }
        // TODO: need to revise --install to be inside of main install method
        else if (std::strcmp(arg, "--install") == 0) {
            // check if the next argument exists
            if (i + 1 < argc) {
                // assuming the next argument is the package name
                individual_package.name = argv[i + 1];
                return PossibleOptions::INSTALL;
            } else {
                std::cerr << "Error: Missing package name after '--install'." << std::endl;
                return PossibleOptions::NONE;
            }
        } else {
            return command_map[arg];
        }
    }

    // no valid options found
    return PossibleOptions::NONE;
}

bool PackageManager::show_warning() {
    // TODO: on pressing no, command still runs -- need to fix
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

void create_row_help_menu(const std::string& command_flag, const std::string& description) {
    std::cout << std::setw(20) << std::left << command_flag;
    std::cout << std::setw(60) << std::right << description << std::endl;
}

void PackageManager::print_help() {
    std::cout << "\n\n\n";
    create_row_help_menu("COMMAND:", "DESCRIPTION:");
    std::cout << std::setw(80) << std::left << "--------------------------------------------------------------------------------" << std::endl;
    create_row_help_menu("--init", "to initialize new project");
    create_row_help_menu("--list", "to list installed packages");
    create_row_help_menu("--install", "to install packages");
    create_row_help_menu("--uninstall", "to uninstall packages");  // TODO: not created yet
    create_row_help_menu("--help", "to show commands");
    create_row_help_menu("--clean", "to remove contents from cpp_libs folder");
    std::cout << "\n\n"
              << std::endl;
}

#endif