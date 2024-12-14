#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"
#include "header_files/package_manager.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int beldum_uninstall(std::string &requested_package,
                     std::string &single_package_directory_path,
                     const std::string &cmake_list_path) {
    using json = nlohmann::ordered_json;
    BeldumLogging logger;

    std::ifstream packages_file;
    std::ifstream installed_packages_file;
    std::ofstream output;

    // values needed from json
    std::string command;
    std::string package_name;
    std::string package_type;
    std::string package_cmake_alias;
    std::vector<std::string> uninstall_instructions;

    json installed_data;
    json package_data;
    int return_code;

    /**
     * CMAKE CONFIG
     */
    std::ifstream cmake_list_file;
    std::vector<std::string> cmakeLines; // used for reading all lines
    std::string cmakeLine;               // used for single cmake line

    installed_packages_file.open(beldum_json_path);
    if (!installed_packages_file.is_open()) {
        logger.logError("Error: Failed to open " + beldum_json_path + " file.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened " + beldum_json_path + " file.");
    installed_data = json::parse(installed_packages_file);
    installed_packages_file.close();

    if (installed_data["dependencies"].contains(requested_package)) {
        logger.log("Uninstalling package: " + requested_package);
        package_name = requested_package;

        // parse packages_file to get repo_type
        packages_file.open(single_package_directory_path);
        if (!packages_file.is_open()) {
            logger.logError("Error: Failed to open " + single_package_directory_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + single_package_directory_path + " file.");

        package_data = json::parse(packages_file);
        packages_file.close();
        package_type = package_data[requested_package]["repo_type"];
        package_cmake_alias = package_data[requested_package]["cmake_alias"];
        uninstall_instructions = package_data[requested_package]["uninstall_instructions"];

        // Remove the repo
        command = "rm -rf target/debug/deps/" + std::string(requested_package);
        logger.log("Executing command: " + command);
        return_code = execute_command_with_spinner(command.c_str());

        output.open(beldum_json_path);
        if (!output.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + beldum_json_path + " file.");

        // Update beldum.json
        installed_data["dependencies"].erase(requested_package);

        output << installed_data.dump(4);
        output.close();

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

        //   findAndInsert(cmakeLines, "BELDUM-STATIC-ONLY", cmakeStaticCommand, repo_name);
        while (std::getline(cmake_list_file, cmakeLine)) {
            cmakeLines.push_back(cmakeLine); // Add the current line to the vector
        }

        /**
         * Logic for when / where to insert in CMakeLists.txt
         */

        if (package_type == "header-only") {
            // if beldum header config is found in CMakeLists.txt
            for (std::string &sentence : cmakeLines) {
                if (sentence.find(build_header_only_library_cmake_input(package_name)) !=
                    std::string::npos) {
                    // Erase after the current sentence in the vector
                    auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                    cmakeLines.erase(it); // Erase the found sentence
                    break;
                }
            }
        }

        if (package_type == "static") {
            // if beldum static command config is found in CMakeLists.txt
            for (std::string &sentence : cmakeLines) {
                if (sentence.find(build_static_library_cmake_input(package_name)) !=
                    std::string::npos) {
                    // Erase after the current sentence in the vector
                    auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                    cmakeLines.erase(it); // Erase the found sentence
                    break;
                }
            }
            // if beldum linker already exists, add to linker
            for (std::string &sentence : cmakeLines) {
                if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                  package_cmake_alias + ")") != std::string::npos) {
                    auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                    cmakeLines.erase(it);
                }
            }
        }

        if (package_type == "dynamic") {
            // To remove dyamic library from OS system
            if (show_warning("WARNING: This operation will remove the dynamic library from the "
                             "system.\nAll libraries that depend on this library will stop "
                             "working. \nSelect 'n' to just remove from current project.")) {
                for (std::string &sentence : cmakeLines) {
                    if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                      package_cmake_alias + ")") != std::string::npos) {
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        cmakeLines.erase(it);
                    }
                }
                for (int i = 0; i < uninstall_instructions.size(); i++) {
                    execute_system_command(uninstall_instructions[i].c_str());
                }
            } else {
                // else remove just from CMakeLists.txt for current project
                for (std::string &sentence : cmakeLines) {
                    if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                      package_cmake_alias + ")") != std::string::npos) {
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        cmakeLines.erase(it);
                    }
                }
            };
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

        logger.log("Package " + requested_package + " successfully uninstalled.");
        fmt::print("\nPackage {} successfully uninstalled.\n\n", requested_package);
    } else {
        logger.logError("Attempted to uninstall package: " + requested_package +
                        ", but it was not found.");
        std::cout << "\n\nPackage " << requested_package
                  << " does not exist. Please check the installed packages and try again...\n\n"
                  << std::endl;
        return_code = 1;
        return return_code;
    }
    return return_code;
};