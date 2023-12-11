#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include <fstream>
#include "../cpp_libs/json/single_include/nlohmann/json.hpp"

#include "headerfiles/package_manager.hpp"

/**
 * TODO: 
 * - figure out a way to not depend on the json library due to time it takes to download library
 * - setup table with commands
 * - create function list of all currently installed packages
 * 
*/


int main(int argc, char* argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);
    instance.check_passed_shell_arguments(options);

    std::string jsonData = R"(
        {
            "name": "John",
            "age": 30,
            "city": "New York",
            "isStudent": false,
            "grades": [90, 85, 92],
            "address": {
                "street": "123 Main St",
                "zip": "10001"
            },
            "isNull": null
        }
    )";

    std::ifstream package_list("cdm_packages.json");
    using json = nlohmann::json;
    json data = json::parse(package_list);

    std::cout << data.dump(4) << std::endl;

    return 0;
}