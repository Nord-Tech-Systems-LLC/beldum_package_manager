

#include "header_files/beldum_install.hpp"
#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"
#include "header_files/package_manager.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

int beldum_install(std::string &requested_package,
                   std::string &repo_name,
                   std::string &repo_cmake_alias,
                   std::string &repo_version,
                   std::string &repository_URL,
                   std::string &repo_type,
                   std::string &command,
                   std::string &single_package_directory_path,
                   const std::string &cmake_list_path,
                   std::string &cmakeStaticCommand,
                   std::string &cmakeHeaderOnlyCommand) {

    using json = nlohmann::ordered_json;
    BeldumLogging logger;
    std::ifstream packages_file;
    std::ifstream installed_packages_file;
    std::ofstream output;

    json package_data;
    json installed_data;
    int return_code;

    std::string result_string;
    /**
     * CMAKE CONFIG
     */
    std::ifstream cmake_list_file;
    std::vector<std::string> cmakeLines; // used for reading all lines
    std::string cmakeLine;               // used for single cmake line
    bool cmakeRunNewLibrary =
        true; // used to dictate if a package has been insertted yet during rotation

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
        repo_name = requested_package;
        repository_URL = package_data[requested_package]["repository_url"];
        repo_type = package_data[requested_package]["repo_type"];
        repo_cmake_alias = package_data[requested_package]["cmake_alias"];
        logger.log("Found package \"" + repo_name + "\" in package.json with repository URL: " +
                   repository_URL + " Type: " + repo_type);
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

    // if cache directory exists
    std::string cache_path = std::string(getenv("HOME")) + "/.beldum/cache/" + repo_name;
    std::string target_path = "target/debug/deps/" + repo_name;

    if (file_exists(cache_path)) {
        // If cache exists, ensure target directory exists
        std::string create_target_dir_command = fmt::format("mkdir -p {}", "target/debug/deps/");
        return_code = system(create_target_dir_command.c_str());

        // copy the repo from cache to the target directory
        std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
        logger.log("Executing command to copy repository from cache: " + copy_command);
        return_code = execute_command_with_spinner(copy_command.c_str());

        if (return_code != 0) { // If cache exists, ensure target directory exists
            create_target_dir_command = fmt::format("mkdir -p {}", "target/debug/deps/");
            return_code = system(create_target_dir_command.c_str());

            // copy the repo from cache to the target directory
            copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
            logger.log("Executing command to copy repository from cache: " + copy_command);
            return_code = system(copy_command.c_str());

            if (return_code != 0) {
                logger.log("Error copying repository from cache.");
                return return_code;
            } else {
                logger.log("Repository copied successfully from cache.");
            }
            logger.log("Error copying repository from cache.");
            return return_code;
        } else {
            logger.log("Repository copied successfully from cache.");
        }
    } else {
        // If cache doesn't exist, clone the repo into the cache directory
        logger.log("Repository not found in cache, cloning repository.");

        std::string clone_command = fmt::format("git clone {} {}", repository_URL, cache_path);
        logger.log("Executing command to clone repository into cache: " + clone_command);
        return_code = system(clone_command.c_str());

        if (return_code != 0) {
            logger.log("Error cloning repository.");
            return return_code;
        }
    }

    // Ensure target directory exists before copying
    std::string create_target_dir_command = fmt::format("mkdir -p {}", "target/debug/deps/");
    return_code = system(create_target_dir_command.c_str());

    if (return_code != 0) {
        logger.log("Error creating target directory.");
        return return_code;
    }

    // After cloning, copy from cache to target/debug/deps/
    std::string copy_command = fmt::format("cp -r {} {}", cache_path, target_path);
    logger.log("Executing command to copy repository from cache: " + copy_command);
    return_code = system(copy_command.c_str());

    if (return_code != 0) {
        logger.log("Error copying repository from cache.");
        return return_code;
    } else {

        logger.log("Repository copied successfully from cache.");
    }

    // git version number
    command = fmt::format("cd {}/target/debug/deps/{} && git describe --tags --abbrev=0",
                          result_string,
                          std::string(repo_name));
    repo_version = execute_command(command);
    repo_version.erase(std::remove(repo_version.begin(), repo_version.end(), '\n'),
                       repo_version.end()); // removes new line character from version

    // assigning unknown if no version number exists in git repo
    if (repo_version.empty()) {
        repo_version = "unknown";
        logger.logWarning("No version tag found in repository. Defaulting to version: unknown.");
    } else {
        logger.log("Retrieved version for " + repo_name + ": " + repo_version);
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

        installed_data["dependencies"][repo_name] = repo_version;

        output << installed_data.dump(4);
        output.close();

        logger.log("Successfully updated " + beldum_json_path + " with " + repo_name);
    } catch (const std::exception &e) {
        logger.logError("Failed to update " + beldum_json_path + ": " + std::string(e.what()));
        return_code = 1;
        return return_code;
    }

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

    // commands for adding
    cmakeStaticCommand = fmt::format("add_subdirectory(${{BELDUM_LIB_DIR}}/{})", repo_name);
    cmakeHeaderOnlyCommand = fmt::format("include_directories(${{BELDUM_LIB_DIR}}/{})", repo_name);

    //   findAndInsert(cmakeLines, "BELDUM-STATIC-ONLY", cmakeStaticCommand, repo_name);
    while (std::getline(cmake_list_file, cmakeLine)) {
        cmakeLines.push_back(cmakeLine); // Add the current line to the vector
    }

    /**
     * Logic for when / where to insert in CMakeLists.txt
     */
    if (repo_type == "header-only") {
        // if beldum header config is found in CMakeLists.txt
        for (std::string &sentence : cmakeLines) {
            if (sentence.find("BELDUM-HEADER-ONLY") != std::string::npos) {
                // Insert after the current sentence in the vector
                auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                cmakeLines.insert(it + 1,
                                  cmakeHeaderOnlyCommand); // Insert after the found sentence
                break; // Optional: if you only want to insert once, otherwise remove this line
            }
        }
    }

    if (repo_type == "static") {
        // if beldum linker already exists, add to linker
        for (std::string &sentence : cmakeLines) {
            if (sentence.find("BELDUM-LINKER") != std::string::npos) {
                auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                cmakeLines.insert(it + 2, "    " + repo_name + "::" + repo_name);
                cmakeRunNewLibrary = false;
            }
        }
        // if beldum library isn't new
        if (!cmakeRunNewLibrary) {
            for (std::string &sentence : cmakeLines) {
                if (sentence.find("BELDUM-STATIC-ONLY") != std::string::npos) {
                    auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                    cmakeLines.insert(it + 1, cmakeStaticCommand);
                    break;
                }
            }
        }
        // if beldum library is new
        if (cmakeRunNewLibrary) {
            for (std::string &sentence : cmakeLines) {
                if (sentence.find("BELDUM-STATIC-ONLY") != std::string::npos) {
                    // Insert after the current sentence in the vector
                    auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);

                    cmakeLines.insert(it + 1,
                                      cmakeStaticCommand); // Insert the static library command
                    cmakeLines.insert(it + 2, "# BELDUM-LINKER"); // Used as a separator
                    cmakeLines.insert(it + 3, "set(MY_LIBRARIES # List your libraries to link");
                    cmakeLines.insert(it + 4, "    " + repo_cmake_alias);
                    cmakeLines.insert(it + 5, ")");

                    break;
                }
            }
        }
    }
    cmake_list_file.close();

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
    for (const auto &outputLine : cmakeLines) {
        output << outputLine << '\n';
    }
    output.close();

    logger.log("Successfully updated " + beldum_json_path + " with " + repo_name);
    std::cout << std::endl;
    fmt::print("{} successfully installed.", repo_name);
    std::cout << std::endl;

    return 0;
}
