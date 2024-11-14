#include "header_files/beldum_list.hpp"
#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"

#include "fmt/core.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

int beldum_list_available(std::string &available_packages_path) {
    using json = nlohmann::json;
    BeldumLogging logger;
    std::ifstream packages_file;
    json package_data;

    int return_code = 0;

    // Check if the available packages file exists
    if (file_exists(available_packages_path)) {
        packages_file.open(available_packages_path);
        if (!packages_file.is_open()) {
            logger.logError("Error: Failed to open available_packages.json file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened available_packages.json file.");
        package_data = json::parse(packages_file);
        packages_file.close();

        // Print header with alignment and divider
        fmt::print("\n{:<20} {:<40} {:<60}\n", "PACKAGE", "DESCRIPTION", "REPOSITORY URL");
        fmt::print("{:<20} {:<40} {:<60}\n",
                   std::string(20, '-'),
                   std::string(40, '-'),
                   std::string(60, '-'));

        // Iterate over each package in the JSON data
        for (const auto &package_entry : package_data["packages"].items()) {
            const std::string &package_name = package_entry.key();
            const json &package = package_entry.value();

            // Retrieve package information -- print package details with alignment
            fmt::print("{:<20} {:<40} {:<60}\n",
                       package_name,
                       std::string(package["description"]),
                       std::string(package["repository_url"]));

            // // Print tags as a comma-separated list, indented
            // if (package.contains("tags")) {
            //     fmt::print("  Tags: {}\n", fmt::join(package["tags"].begin(),
            //     package["tags"].end(), ", "));
            // }
        }

        // Add spacing after the output
        fmt::print("\n\n");
    } else {
        // Display error if the file is missing
        std::cerr << "\nThe available_packages.json does not exist. Please ensure the file is "
                     "available at ~/.beldum/packages/\n";
        return_code = 1;
        return return_code;
    }

    return return_code;
};

int beldum_list_installed(std::string &installed_packages_path) {
    using json = nlohmann::json;
    BeldumLogging logger;
    std::ifstream installed_packages_file;
    json installed_data;

    int return_code = 1;

    if (file_exists("installed_packages.json")) {
        installed_packages_file.open(installed_packages_path);
        if (!installed_packages_file.is_open()) {
            logger.logError("Error: Failed to open installed_packages.json file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened installed_packages.json file.");
        installed_data = json::parse(installed_packages_file);
        installed_packages_file.close();

        // Print a header with better alignment and a divider
        fmt::print("\n{:<40} {:<20}\n", "PACKAGE", "VERSION");
        fmt::print("{:<40} {:<20}\n", std::string(40, '-'), std::string(20, '-'));

        // Iterate over installed packages and display each one
        for (const auto &package : installed_data["packages"]) {
            // std::string repo_name = package.value()["repo_name"];
            // std::string version = package.value()["version"];
            fmt::print("{:<40} {:<20}\n",
                       std::string(package["repo_name"]),
                       std::string(package["version"]));
        }

        // Bottom spacing
        fmt::print("\n\n");
    } else {
        std::cerr << "\nThe installed_packages.json does not exist, please run --init" << std::endl;
        return_code = 1;
        return return_code;
    }
    return return_code;
};