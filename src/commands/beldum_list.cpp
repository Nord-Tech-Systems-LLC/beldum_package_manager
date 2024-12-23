#include "header_files/beldum_list.hpp"
#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"

#include "fmt/core.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <map>

// Function to parse a single JSON file
nlohmann::ordered_json parse_json_file(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error("JSON parsing error in file " + filepath + ": " +
                                 std::string(e.what()));
    }
    return jsonData;
}

// Main function to parse all JSON files in a directory
std::map<std::string, nlohmann::ordered_json>
parse_all_json_files(const std::string &directoryPath) {
    std::map<std::string, nlohmann::ordered_json> json_files_data;

    for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            const std::string filename = entry.path().filename().string();
            try {
                nlohmann::json json_data = parse_json_file(entry.path().string());

                // Add the parsed JSON to the map
                json_files_data[filename] = json_data;

            } catch (const std::exception &e) {
                std::cerr << "Error parsing " << filename << ": " << std::string(e.what()) << '\n';
            }
        }
    }
    return json_files_data;
}

int beldum_list_available() {
    using json = nlohmann::ordered_json;
    BeldumLogging logger;
    std::ifstream packages_file;
    json package_data;

    int return_code = 0;

    // Check if the available packages file exists
    if (file_exists(available_packages_path)) {
        // const std::string directoryPath = "."; // Use the current directory
        auto json_files_data = parse_all_json_files(available_packages_path);

        // Print header with alignment and divider
        fmt::print("\n{:<20} {:<40} {:<60}\n", "PACKAGE", "DESCRIPTION", "REPOSITORY URL");
        fmt::print("{:<20} {:<40} {:<60}\n",
                   std::string(20, '-'),
                   std::string(40, '-'),
                   std::string(60, '-'));

        // Example output of parsed data
        for (const auto &[filename, json_data] : json_files_data) {
            for (const auto &package_entry : json_data.items()) {
                const std::string &package_name = package_entry.key();
                const json &package = package_entry.value();

                std::string description = package["description"];
                std::string truncated_description =
                    description.length() > 37 ? description.substr(0, 36) + "..." : description;

                // Retrieve package information -- print package details with alignment
                fmt::print("{:<20} {:<40} {:<60}\n",
                           package_name,
                           truncated_description,
                           std::string(package["repository_url"]));

                // // Print tags as a comma-separated list, indented
                // if (package.contains("tags")) {
                //     fmt::print("  Tags: {}\n", fmt::join(package["tags"].begin(),
                //     package["tags"].end(), ", "));
                // }
            };
            //  std::cout << "Contents of " << filename << ":\n"
            //                            << jsonData.dump(4) << "\n\n";
        }

        // Add spacing after the output
        fmt::print("\n\n");
    } else {
        // Display error if the file is missing
        fmt::print("\nThe {} path does not exist. Please ensure {} exists.\n",
                   available_packages_path,
                   available_packages_path);
        return_code = 1;
        return return_code;
    }

    return return_code;
};

int beldum_list_installed() {
    using json = nlohmann::ordered_json;
    BeldumLogging logger;
    std::ifstream installed_packages_file;
    json installed_data;

    int return_code = 1;

    if (file_exists(beldum_json_path)) {
        installed_packages_file.open(beldum_json_path);
        if (!installed_packages_file.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + beldum_json_path + " file.");
        installed_data = json::parse(installed_packages_file);
        installed_packages_file.close();

        // Print a header with better alignment and a divider
        fmt::print("\n{:<40} {:<20}\n", "PACKAGE", "VERSION");
        fmt::print("{:<40} {:<20}\n", std::string(40, '-'), std::string(20, '-'));

        // Iterate over installed packages and display each one
        for (const auto &package : installed_data["dependencies"].items()) {
            // std::string repo_name = package.value()["repo_name"];
            // std::string version = package.value()["version"];
            fmt::print("{:<40} {:<20}\n", std::string(package.key()), std::string(package.value()));
        }

        // Bottom spacing
        fmt::print("\n\n");
    } else {
        fmt::print("\nThe {} does not exist, please run --init", beldum_json_path);
        std::cerr << std::endl;
        return_code = 1;
        return return_code;
    }
    return return_code;
};