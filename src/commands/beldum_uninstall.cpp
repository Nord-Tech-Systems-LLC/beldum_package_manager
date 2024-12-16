#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"
#include "header_files/package_manager.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int beldum_uninstall(std::string &requested_package,
                     std::string &single_package_directory_path,
                     const std::string &cmake_list_path) {
    // init needed variables for switch
    nlohmann::ordered_json installed_data;
    nlohmann::ordered_json package_data;
    BeldumLogging logger;

    // values needed from json
    std::string command;
    std::string package_name;
    std::string package_type;
    std::string package_cmake_alias;
    std::unordered_map<std::string, std::vector<std::string>> instructions;

    // cmake config
    std::vector<std::string> cmake_lines; // used for reading all lines

    int return_code;

    // parse beldum json file to get JSON data
    if (!open_file_parse_json(beldum_json_path, installed_data)) {
        return 1;
    };

    if (installed_data["dependencies"].contains(requested_package)) {
        logger.log("Uninstalling package: " + requested_package);
        package_name = requested_package;

        // parse packages_file to get data from JSON
        if (!open_file_parse_json(single_package_directory_path, package_data)) {
            return 1;
        };

        package_type = package_data[requested_package]["repo_type"];
        package_cmake_alias = package_data[requested_package]["cmake_alias"];
        instructions = package_data[requested_package]["instructions"];
        std::vector<std::string> uninstall_instructions = instructions["uninstall"];

        // Remove the repo
        command = "rm -rf target/debug/deps/" + std::string(requested_package);
        logger.log("Executing command: " + command);
        return_code = execute_command_with_spinner(command.c_str());

        // Remove requested package from beldum.json
        installed_data["dependencies"].erase(requested_package);
        if (!write_json_to_file(beldum_json_path, installed_data)) {
            return 1;
        };

        // read the file line by line to vector for manipulating
        if (!read_file_to_vector(cmake_list_path, cmake_lines)) {
            return 1;
        };

        // uninstall instructions for header only
        if (package_type == "header-only") {
            // if beldum header config is found in CMakeLists.txt
            for (std::string &sentence : cmake_lines) {
                if (sentence.find(build_header_only_library_cmake_input(package_name)) !=
                    std::string::npos) {
                    // Erase after the current sentence in the vector
                    auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                    cmake_lines.erase(it); // Erase the found sentence
                    break;
                }
            }
        }
        // uninstall instructions for static
        if (package_type == "static") {
            // if beldum static command config is found in CMakeLists.txt
            for (std::string &sentence : cmake_lines) {
                if (sentence.find(build_static_library_cmake_input(package_name)) !=
                    std::string::npos) {
                    // Erase after the current sentence in the vector
                    auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                    cmake_lines.erase(it); // Erase the found sentence
                    break;
                }
            }
            // if beldum linker already exists, add to linker
            for (std::string &sentence : cmake_lines) {
                if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                  package_cmake_alias + ")") != std::string::npos) {
                    auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                    cmake_lines.erase(it);
                }
            }
        }
        // uninstall instructions for dynamic
        if (package_type == "dynamic") {
            // To remove dyamic library from OS system
            if (show_warning("WARNING: This operation will remove the dynamic library from the "
                             "system.\nAll libraries that depend on this library will stop "
                             "working. \nSelect 'n' to just remove from current project.")) {
                for (std::string &sentence : cmake_lines) {
                    if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                      package_cmake_alias + ")") != std::string::npos) {
                        auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                        cmake_lines.erase(it);
                    }
                }
                for (int i = 0; i < uninstall_instructions.size(); i++) {
                    execute_system_command(uninstall_instructions[i].c_str());
                }
            } else {
                // else remove just from CMakeLists.txt for current project
                for (std::string &sentence : cmake_lines) {
                    if (sentence.find("target_link_libraries(${EXECUTABLE_NAME} PRIVATE " +
                                      package_cmake_alias + ")") != std::string::npos) {
                        auto it = std::find(cmake_lines.begin(), cmake_lines.end(), sentence);
                        cmake_lines.erase(it);
                    }
                }
            };
        }

        // write to CMakeLists.txt
        if (!write_vector_to_file(cmake_list_path, cmake_lines)) {
            return 1;
        };

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