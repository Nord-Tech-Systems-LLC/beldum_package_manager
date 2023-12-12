#include "headerfiles/package_manager.hpp"

#include <algorithm>
#include <fstream>

#include "../cpp_libs/json/single_include/nlohmann/json.hpp"

Package individual_package;

void PackageManager::check_passed_shell_arguments(PossibleOptions options) {
    // read shell arguments passed
    int return_code;
    std::string command;
    std::string repository_URL;
    std::string repo_name;

    // gets list of packages
    std::ifstream packages_file("cdm_packages.json");

    // instantiate json object
    using json = nlohmann::json;
    json data;

    std::string requested_package = individual_package.name;

    switch (options) {
        /**
         * HELP ACTIONS
         */
        case PossibleOptions::HELP:
            std::cout << "Help printed" << std::endl;
            break;

        /**
         * INSTALL ACTIONS
         */
        case PossibleOptions::INSTALL:
            data = json::parse(packages_file);

            std::cout << data.dump(4) << std::endl;

            repo_name = data["packages"][requested_package]["repo_name"];
            repository_URL = data["packages"][requested_package]["git_link"];

            // unsure if this is needed yet
            // std::cout.flush();

            // using bash to clone the repo
            command = "git clone " + std::string(repository_URL) + " cpp_libs/" + std::string(repo_name);
            std::cout << "Command: " << command << std::endl;
            return_code = system(command.c_str());

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

void PackageManager::print_table(const std::vector<Package>& package) {
    // Determine column widths
    std::vector<size_t> column_widths = {0, 0, 0};

    for (const auto& pm : package) {
        column_widths[0] = std::max(column_widths[0], pm.name.length());
        column_widths[1] = std::max(column_widths[1], pm.version.length());
        column_widths[2] = std::max(column_widths[2], pm.description.length());
    }

    // Print table header
    std::cout << std::left << std::setw(column_widths[0] + 2) << "Name"  // Added 2 for padding
              << std::setw(column_widths[1] + 4) << "Version"            // Added 4 for padding
              << std::setw(column_widths[2] + 2) << "Description"        // Added 2 for padding
              << std::endl;

    // Print table data
    for (const auto& pm : package) {
        std::cout << std::setw(column_widths[0] + 2) << pm.name         // Added 2 for padding
                  << std::setw(column_widths[1] + 4) << pm.version      // Added 4 for padding
                  << std::setw(column_widths[2] + 2) << pm.description  // Added 2 for padding
                  << std::endl;
    }
}