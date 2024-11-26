#include "header_files/beldum_logging.hpp"
#include "header_files/global_utilities.hpp"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

int beldum_clean(std::string &command) {
    using json = nlohmann::ordered_json;
    BeldumLogging logger;
    std::ofstream output;

    json installed_data;
    int return_code = 0;

    // using bash to clean the library folder
    if (show_warning()) {
        command =
            "sudo find target/debug/deps/* -maxdepth 0 -type d ! -name \"json\" -exec rm -r {} +";

        output.open(beldum_json_path);
        if (!output.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened " + beldum_json_path + " file.");
        installed_data["packages"] = {};
        output << installed_data;

        return_code = system(command.c_str());
        fmt::print("\nSuccessfully cleaned project directory.\n");
        std::cout << std::endl;
    } else {
        return_code = 1;
        return return_code;
    };

    return return_code;
};