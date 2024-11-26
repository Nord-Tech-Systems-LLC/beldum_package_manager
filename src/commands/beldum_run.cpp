#include "header_files/global_utilities.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <cstdlib> // for std::system

namespace beldum_run {
enum ScriptType { BashScript, SystemCommand, Unknown };

ScriptType detect_script_type(std::string &script) {
    // to check if script is BashScript
    if (starts_with(script, "./") && ends_with(script, ".sh")) {
        if (std::filesystem::exists(script) && std::filesystem::is_regular_file(script)) {
            return BashScript;
        }
    } else {
        return SystemCommand;
    }
    return Unknown;
}
} // namespace beldum_run

int execute_build_script(std::string &script_name) {
    std::string beldum_json_path = "beldum.json";
    std::ifstream beldum_json_file;
    using json = nlohmann::json;
    int return_code = 0;

    std::cout << script_name << std::endl;
    json beldum_json_data;

    // open and parse beldum.json
    beldum_json_file.open(beldum_json_path);
    if (!beldum_json_file.is_open()) {
        std::cerr << "Error: there was an issue opening beldum.json" << std::endl;
        return 1;
    }
    beldum_json_data = json::parse(beldum_json_file);
    beldum_json_file.close();

    // Check if the script exists
    if (beldum_json_data.contains("scripts") && beldum_json_data["scripts"].contains(script_name)) {
        std::string command = beldum_json_data["scripts"][script_name];
        beldum_run::ScriptType script_type = beldum_run::detect_script_type(command);

        switch (script_type) {
        case beldum_run::BashScript:
            std::cout << "Detected Bash script. Executing with bash..." << std::endl;
            return_code = std::system(("bash " + command).c_str());
            break;

        case beldum_run::SystemCommand:
            std::cout << "Detected system command. Executing directly..." << std::endl;
            if (script_name == "clean") {
                if (show_warning()) {
                    std::cout << "Running script: " << script_name << " -> " << command
                              << std::endl;
                    return_code = std::system(command.c_str());
                    break;
                };
            } else {
                std::cout << "Running script: " << script_name << " -> " << command << std::endl;
                return_code = std::system(command.c_str());
                break;
            }
            return return_code;

        default:
            std::cerr << "Error: Unknown script type or command not found: " << command
                      << std::endl;
            return_code = 1;
            break;
        }
        return return_code;
    } else {
        std::cerr << "Error: Script '" << script_name << "' not found in metadata." << std::endl;
        return_code = 1;
        return return_code;
    }

    return 0;
}