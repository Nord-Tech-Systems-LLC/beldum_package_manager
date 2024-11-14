

#include "headerfiles/beldum_install.hpp"
#include "headerfiles/beldum_logging.hpp"
#include "headerfiles/global_utilities.hpp"
#include "headerfiles/package_manager.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

int beldum_install(std::string &requested_package,
                   std::string &repo_name,
                   std::string &repo_version,
                   std::string &repository_URL,
                   std::string &repo_type,
                   std::string &command,
                   std::string &available_packages_path,
                   std::string &installed_packages_path,
                   const std::string &cmake_list_path,
                   std::string &cmakeStaticCommand,
                   std::string &cmakeHeaderOnlyCommand) {

    using json = nlohmann::json;
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
    std::string testing = std::string(std::filesystem::current_path());
    logger.log("Current directory path: " + testing);
    logger.log("Attempting to install package: " + requested_package);

    // Parse package.json and installed_packages.json
    try {
        packages_file.open(available_packages_path);
        if (!packages_file.is_open()) {
            logger.logError("Error: Failed to open available_packages.json file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened available_packages.json file.");
        package_data = json::parse(packages_file);
        packages_file.close();

        installed_packages_file.open(installed_packages_path);
        if (!installed_packages_file.is_open()) {
            logger.logError("Error: Failed to open installed_packages.json file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened installed_packages.json file.");
        installed_data = json::parse(installed_packages_file);
        installed_packages_file.close();

        logger.log("Parsed JSON data from package files.");
    } catch (const json::parse_error &e) {
        logger.logError("Failed to parse JSON data: " + std::string(e.what()));
        return_code = 1;
        return return_code;
    }

    // Check if package is already installed
    if (installed_data["packages"].contains(requested_package)) {
        logger.logWarning("The package \"" + requested_package + "\" is already installed.");
        fmt::print("\nThe package \"{}\" is already installed...\n\n", requested_package);
        return_code = 1;
        return return_code;
    }

    // checks if package exists in package.json
    if (package_data["packages"].contains(requested_package)) {

        repo_name = requested_package;
        repository_URL = package_data["packages"][requested_package]["repository_url"];
        repo_type = package_data["packages"][requested_package]["repo_type"];
        logger.log("Found package \"" + repo_name + "\" in package.json with repository URL: " +
                   repository_URL + "Type: " + repo_type);

        // strips quotes from directory
        for (char c : testing) {
            if (c != '\"') {
                result_string += c;
            }
        }
        logger.log("Cleaned directory path: " + result_string);

        // clone the repo
        command = fmt::format("cd {} && git clone {} target/debug/deps/{}",
                              result_string,
                              std::string(repository_URL),
                              std::string(repo_name));
        logger.log("Executing command to clone repository: " + command);
        return_code = system(command.c_str());

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
            logger.logWarning(
                "No version tag found in repository. Defaulting to version: unknown.");
        } else {
            logger.log("Retrieved version for " + repo_name + ": " + repo_version);
        }
        // Update installed package manager
        try {
            output.open(installed_packages_path);
            if (!output.is_open()) {
                logger.logError("Error: Failed to open installed_packages.json file.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened installed_packages.json file.");

            installed_data["packages"][repo_name] = {{"git_link", repository_URL},
                                                     {"repo_name", repo_name},
                                                     {"version", repo_version},
                                                     {"repo_type", repo_type}};

            output << installed_data.dump(4);
            output.close();

            logger.log("Successfully updated installed_packages.json with " + repo_name);
        } catch (const std::exception &e) {
            logger.logError("Failed to update installed_packages.json: " + std::string(e.what()));
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
        cmakeHeaderOnlyCommand =
            fmt::format("include_directories(${{BELDUM_LIB_DIR}}/{})", repo_name);

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
                        cmakeLines.insert(it + 4, "    " + repo_name + "::" + repo_name);
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

        logger.log("Successfully updated installed_packages.json with " + repo_name);
    } else {
        logger.logWarning("Package \"" + requested_package + "\" does not exist in package.json.");
        fmt::print("\nPackage \"{}\" does not exist. Please enter the correct package name.\n\n",
                   requested_package);
    }

    return 0;
}
