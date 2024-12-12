

#include "header_files/beldum_install.hpp"
#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"
#include "header_files/package_manager.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

void process_static_lib(const std::string &package_name,
                        const std::string &package_URL,
                        const std::string &package_cmake_alias,
                        std::vector<std::string> &cmake_lines) {
    std::string cache_path = std::string(getenv("HOME")) + "/.beldum/cache/" + package_name;
    std::string dependency_path = "target/debug/deps/";
    std::string target_path = dependency_path + package_name;

    try {
        // Step 1: Check if project dependency directory exists
        std::string create_target_dir_command = fmt::format("mkdir -p {}", dependency_path);
        execute_system_command(create_target_dir_command.c_str());

        // Step 2: Check if static lib is downloaded in cache directory
        if (file_exists(cache_path)) {

            // Step 3: copy from cache directory into project deps directory
            std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
            execute_command_with_spinner(copy_command.c_str());

        } else {
            // Step 4: else if static lib is downloaded in cache directory

            // Step 5: clone from git into cache directory
            std::string clone_command = fmt::format("git clone {} {}", package_URL, cache_path);
            execute_system_command(clone_command.c_str());

            // Step 6: copy from cache directory into project deps directory
            std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
            execute_system_command(copy_command.c_str());
        }

        // if beldum linker already exists, add to linker
        for (std::string &sentence : cmake_lines) {
            if (sentence.find("BELDUM-LINKER") != std::string::npos) {
                auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                cmake_lines.insert(it + 1,
                                   "target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                       package_cmake_alias + ")");
            }
        }

        // command to add sub directory
        for (std::string &sentence : cmake_lines) {
            if (sentence.find("BELDUM-STATIC-ONLY") != std::string::npos) {
                auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                cmake_lines.insert(it + 1, build_static_library_cmake_input(package_name));
                break;
            }
        }

        std::cout << "Static library " << package_name << " installed successfully.\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error installing static library: " << ex.what() << '\n';
    }
}

void process_header_only_lib(const std::string &package_name,
                             const std::string &package_URL,
                             const std::string &package_cmake_alias,
                             std::vector<std::string> &cmake_lines) {
    std::string cache_path = std::string(getenv("HOME")) + "/.beldum/cache/" + package_name;
    std::string dependency_path = "target/debug/deps/";
    std::string target_path = dependency_path + package_name;

    try {
        // Step 1: Check if project dependency directory exists
        std::string create_target_dir_command = fmt::format("mkdir -p {}", dependency_path);
        execute_system_command(create_target_dir_command.c_str());

        // Step 2: Check if static lib is downloaded in cache directory
        if (file_exists(cache_path)) {

            // Step 3: copy from cache directory into project deps directory
            std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
            execute_command_with_spinner(copy_command.c_str());

        } else {
            // Step 4: else if static lib is downloaded in cache directory

            // Step 5: clone from git into cache directory
            std::string clone_command = fmt::format("git clone {} {}", package_URL, cache_path);
            execute_system_command(clone_command.c_str());

            // Step 6: copy from cache directory into project deps directory
            std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
            execute_system_command(copy_command.c_str());
        }

        // if beldum linker already exists, add to linker
        for (std::string &sentence : cmake_lines) {
            if (sentence.find("BELDUM-LINKER") != std::string::npos) {
                auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                cmake_lines.insert(it + 1,
                                   "target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                       package_cmake_alias + ")");
            }
        }

        // command to add sub directory
        for (std::string &sentence : cmake_lines) {
            if (sentence.find("BELDUM-STATIC-ONLY") != std::string::npos) {
                auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                cmake_lines.insert(it + 1, build_static_library_cmake_input(package_name));
                break;
            }
        }

        std::cout << "Header library " << package_name << " installed successfully.\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error installing static library: " << ex.what() << '\n';
    }
}

void process_dynamic_lib(const std::string &package_name,
                         const std::string &package_cmake_alias,
                         std::vector<std::string> &install_instructions,
                         std::vector<std::string> &cmake_lines) {
    std::string cache_path = std::string(getenv("HOME")) + "/.beldum/cache/" + package_name;
    std::string dependency_path = "target/debug/deps/";
    std::string target_path = dependency_path + package_name;

    try {
        // Step 1: Check if project dependency directory exists
        std::string create_target_dir_command = fmt::format("mkdir -p {}", dependency_path);
        execute_system_command(create_target_dir_command.c_str());

        // Step 2: Install based on install_instructions
        for (int i = 0; i < install_instructions.size(); i++) {
            if (i - 1 == install_instructions.size()) {
                continue;
            } else {
                execute_system_command(install_instructions[i].c_str());
            }
        }

        // if beldum linker already exists, add to linker
        for (std::string &sentence : cmake_lines) {
            if (sentence.find("BELDUM-LINKER") != std::string::npos) {
                auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                cmake_lines.insert(it + 1,
                                   "target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                       package_cmake_alias + ")");
            }
        }

        std::cout << "Dynamic library " << package_name << " installed successfully.\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error installing static library: " << ex.what() << '\n';
    }
}

int beldum_install(std::string &requested_package,
                   std::string &repo_version,
                   std::string &single_package_directory_path,
                   const std::string &cmake_list_path) {

    // init needed variables for switch
    using json = nlohmann::ordered_json;
    json package_data;
    json installed_data;
    BeldumLogging logger;
    std::ifstream packages_file;
    std::ifstream installed_packages_file;
    std::ofstream output;

    // values needed from json
    std::string command;
    std::string package_URL;
    std::string package_name;
    std::string package_type;
    std::string package_cmake_alias;
    std::vector<std::string> install_instructions;

    int return_code;
    std::string result_string;

    // cmake config variables
    std::ifstream cmake_list_file;
    std::vector<std::string> cmake_lines; // used for reading all lines
    std::string cmakeLine;                // used for single cmake line

    // Get and log the current path
    std::string current_path = std::string(std::filesystem::current_path());
    logger.log("Current directory path: " + current_path);
    logger.log("Attempting to install package: " + requested_package);

    // Parse package.json and beldum.json
    try {
        packages_file.open(single_package_directory_path);
        if (!packages_file.is_open()) {
            logger.logError("Error: Failed to open " + single_package_directory_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + single_package_directory_path + " file.");

        package_data = json::parse(packages_file);
        packages_file.close();

        installed_packages_file.open(beldum_json_path);
        if (!installed_packages_file.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + beldum_json_path + " file.");
        installed_data = json::parse(installed_packages_file);
        installed_packages_file.close();

        logger.log("Parsed JSON data from package files.");
    } catch (const json::parse_error &e) {
        logger.logError("Failed to parse JSON data: " + std::string(e.what()));
        return_code = 1;
        return return_code;
    }

    // Check if package is already installed
    if (installed_data["dependencies"].contains(requested_package)) {
        logger.logWarning("The package \"" + requested_package + "\" is already installed.");
        fmt::print("\nThe package \"{}\" is already installed...\n\n", requested_package);
        return_code = 1;
        return return_code;
    }

    // checks if package exists in available_packages
    if (package_data.contains(requested_package)) {
        package_name = requested_package;
        package_URL = package_data[requested_package]["repository_url"];
        package_type = package_data[requested_package]["repo_type"];
        package_cmake_alias = package_data[requested_package]["cmake_alias"];
        install_instructions = package_data[requested_package]["install_instructions"];

        logger.log("Found package \"" + package_name + "\" in package.json with repository URL: " +
                   package_URL + " Type: " + package_type);
    } else {
        logger.logWarning("Package \"" + requested_package + "\" does not exist in package.json.");
        fmt::print("\nPackage \"{}\" does not exist. Please enter the correct package name.\n\n",
                   requested_package);
        return_code = 1;
        return return_code;
    }

    // strips quotes from directory
    for (char c : current_path) {
        if (c != '\"') {
            result_string += c;
        }
    }
    logger.log("Cleaned directory path: " + result_string);

    /**
     * Read the file line by line
     */
    cmake_list_file.open(cmake_list_path);
    if (!cmake_list_file.is_open()) {
        logger.logError("Error: Failed to open CMakeLists.txt file for reading.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened CMakeLists.txt file for reading.");

    while (std::getline(cmake_list_file, cmakeLine)) {
        cmake_lines.push_back(cmakeLine); // Add the current line to the vector
    }
    cmake_list_file.close();

    /**
     * Logic for when / where to insert in CMakeLists.txt
     */
    if (package_type == "header-only") {
        process_static_lib(package_name, package_URL, package_cmake_alias, cmake_lines);
    }

    if (package_type == "static") {
        process_header_only_lib(package_name, package_URL, package_cmake_alias, cmake_lines);
    }

    if (package_type == "dynamic") {
        process_dynamic_lib(package_name, package_cmake_alias, install_instructions, cmake_lines);
    }

    /**
     * Write to CMakeLists.txt
     */
    output.open(cmake_list_path);
    if (!output.is_open()) {
        logger.logError("Error: Failed to open CMakeLists.txt file for writing.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened CMakeLists.txt file for writing.");
    for (const auto &outputLine : cmake_lines) {
        output << outputLine << '\n';
    }
    output.close();

    /**
     * Update version info in beldum.json
     */
    if (package_type == "static" || package_type == "header-only") {
        command = fmt::format("cd {}/target/debug/deps/{} && git describe --tags --abbrev=0",
                              result_string,
                              std::string(package_name));
        repo_version = execute_command_return_result(command);
        repo_version.erase(std::remove(repo_version.begin(), repo_version.end(), '\n'),
                           repo_version.end()); // removes new line character from version
    }

    if (package_type == "dynamic") {
        repo_version =
            execute_command_return_result(install_instructions[install_instructions.size() - 1]);
        repo_version.erase(std::remove(repo_version.begin(), repo_version.end(), '\n'),
                           repo_version.end()); // removes new line character from version
    }

    // assigning unknown if no version number exists in git repo
    if (repo_version.empty()) {
        repo_version = "unknown";
        logger.logWarning("No version tag found in repository. Defaulting to version: unknown.");
    } else {
        logger.log("Retrieved version for " + package_name + ": " + repo_version);
    }
    // Update installed package manager
    try {
        output.open(beldum_json_path);
        if (!output.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + beldum_json_path + " file.");

        installed_data["dependencies"][package_name] = repo_version;

        output << installed_data.dump(4);
        output.close();

        logger.log("Successfully updated " + beldum_json_path + " with " + package_name);
    } catch (const std::exception &e) {
        logger.logError("Failed to update " + beldum_json_path + ": " + std::string(e.what()));
        return_code = 1;
        return return_code;
    }

    logger.log("Successfully updated " + beldum_json_path + " with " + package_name);

    return 0;
}
