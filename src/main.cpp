#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include "json_parser.cpp"
#include "headerfiles/package_manager.hpp"

int main(int argc, char* argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);
    instance.check_passed_shell_arguments(options);

    // PossibleOptions option = parse_arguments(argc, argv);
    // check_passed_shell_arguments(option);

    // Example data
    // std::vector<PackageManager> packageManagers = {
    //     {"PackageManager1", "1.0.0", "Description of PackageManager1"},
    //     {"PackageManager2", "2.1.0", "Description of PackageManager2"},
    //     {"PackageManager3", "3.2.1", "Description of PackageManager3"}};

    // std::string json = R"(
    //     {
    //         "name": "John",
    //         "age": 30,
    //         "city": "New York",
    //         "isStudent": false,
    //         "grades": [90, 85, 92],
    //         "address": {
    //             "street": "123 Main St",
    //             "zip": "10001"
    //         },
    //         "isNull": null
    //     }
    // )";

    // JSONParser parser;
    // JSONValue* root = parser.parse(json);

    // Process the parsed JSON data here

    // Clean up memory
    // delete instance;

    // Print the table
    // printTable(packageManagers);

    return 0;
}