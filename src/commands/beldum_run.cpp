#include "header_files/global_utilities.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <cstdlib> // for std::system

namespace beldum_run {
enum ScriptType { BashScript, SystemCommand };

ScriptType detect_script_type(std::string &script) {
    // to check if script is BashScript
    if (starts_with(script, "./") && ends_with(script, ".sh")) {
        if (std::filesystem::exists(script) && std::filesystem::is_regular_file(script)) {
            return BashScript;
        }
    } else
        return SystemCommand;
}
} // namespace beldum_run

int execute_build_script(std::string &script_name) {
    std::string beldum_json_path = "beldum.json";
    std::ifstream beldum_json_file;
    using json = nlohmann::json;

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
        std::cout << "Running script: " << script_name << " -> " << command << std::endl;

        switch (script_type) {
        case beldum_run::BashScript:
            std::cout << "Detected Bash script. Executing with bash..." << std::endl;
            std::system(("bash " + command).c_str());
            break;

        case beldum_run::SystemCommand:
            std::cout << "Detected system command. Executing directly..." << std::endl;
            std::system(command.c_str());
            break;
        default:
            std::cerr << "Error: Unknown script type or command not found: " << command
                      << std::endl;
            break;
        }
    } else {
        std::cerr << "Error: Script '" << script_name << "' not found in metadata." << std::endl;
        return 1;
    }

    return 0;
}