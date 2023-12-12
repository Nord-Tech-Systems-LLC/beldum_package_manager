#include <sys/stat.h>
#include <sys/types.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include "headerfiles/package_manager.hpp"

// libraries
#include "json/single_include/nlohmann/json.hpp"

/**
 * TODO:
 * - figure out a way to not depend on the json library due to time it takes to download library
 * - setup table with commands
 * - create function list of all currently installed packages
 *
 */

inline bool file_exists(const std::string& name) {
    std::ifstream file(name.c_str());
    return file.good();
}

int main(int argc, char* argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);
    instance.check_passed_shell_arguments(options);

    // std::string jsonData = R"(
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

    // creates packages folder if it doesn't exist
    if (!file_exists("cdm_packages.json")) {
        std::ofstream output("cdm_packages.json");
    }
    std::ifstream package_list("cdm_packages.json");

    // downloads json dependency if it doesn't exist
    struct stat info;
    const char* pathname = "cpp_libs/json";
    if (stat(pathname, &info) != 0) {
        printf("cannot access %s\n", pathname);
    } else if (info.st_mode & S_IFDIR) {  // S_ISDIR() doesn't exist on my windows
        printf("%s is a directory\n", pathname);
    } else {
        std::string = command = "git clone git@github.com:nlohmann/json.git cpp_libs/json";
        printf("%s is no directory\n", pathname);
    }
    // const char* command = R"(
    //     if [ -d "cpp_libs/json" ]; then
    //         echo "json dependency exists"
    //     else
    //         echo "json dependency doesn't exists... please wait while we install."
    //     fi
    // )";
    // int check_for_project_dependency = system(command);

    using json = nlohmann::json;
    json data = json::parse(package_list);

    std::cout << data.dump(4) << std::endl;

    return 0;
}