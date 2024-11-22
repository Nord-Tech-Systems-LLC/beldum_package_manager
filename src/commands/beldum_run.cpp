#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

// TODO: clean script does not work, need to adjust to clean_up

int execute_build_script(std::string &script_name) {
    std::string beldum_json_path = "beldum.json";
    std::ifstream beldum_json_file;
    using json = nlohmann::json;

    std::cout << script_name << std::endl;
    json beldum_json_data;

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
        std::cout << "Running script: " << script_name << " -> " << command << std::endl;

        // Execute the command
        int retCode = std::system(command.c_str());
        if (retCode != 0) {
            std::cerr << "Error: Script '" << script_name << "' failed with code " << retCode
                      << std::endl;
            return retCode;
        } else {
            std::cout << "Script '" << script_name << "' completed successfully." << std::endl;
        }
    } else {
        std::cerr << "Error: Script '" << script_name << "' not found in metadata." << std::endl;
        return 1;
    }

    return 0;
}