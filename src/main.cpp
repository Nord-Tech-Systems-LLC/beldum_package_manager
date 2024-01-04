#include <sys/stat.h>
#include <sys/types.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

/**
 * TODO:
 * - create a script for downloading JSON library dependency
 * - create package update
 * - create package remove
 * - when creating package change the logic for where the cpp_libs file is saved
 * - need to move folder structure to separate backend / frontend
 * - need to figure out where to move cpp_libs so it's based on the current directory and not inside the package manager
 * - setup table with commands
 * - create function list of all currently installed packages
 *
 */

// libraries
#include "headerfiles/package_manager.hpp"

#ifdef JSON_DEPENDENCY_EXIST
#include "json/single_include/nlohmann/json.hpp"

inline bool file_exists(const std::string& name) {
    std::ifstream file(name.c_str());
    return file.good();
}

int main(int argc, char* argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);
    instance.check_passed_shell_arguments(options);

    // creates packages folder if it doesn't exist
    if (!file_exists("installed_packages.json")) {
        std::cout << "This printed..." << std::endl;
        std::ofstream output("installed_packages.json");
    }
    std::ifstream package_list("installed_packages.json");

    // downloads json dependency if it doesn't exist
    // struct stat info;
    // const char* pathname = "cpp_libs/json";
    // if (stat(pathname, &info) != 0) {
    //     printf("cannot access %s\n", pathname);
    // } else if (info.st_mode & S_IFDIR) {  // S_ISDIR() doesn't exist on my windows
    //     printf("%s is a directory\n", pathname);
    // } else {
    //     // std::string = command = "git clone git@github.com:nlohmann/json.git cpp_libs/json";
    //     printf("%s is no directory\n", pathname);
    // }

    using json = nlohmann::json;
    json data = json::parse(package_list);

    // std::cout << data.dump(4) << std::endl;

    return 0;
}

// // handles dependencies needed to execute package manager
// int main() {
//     std::string pre_requisites_command = "git clone git@github.com:nlohmann/json.git cpp_libs/json/";
//     std::cout << "Dependency does not exist... Please wait while the json library is installed." << std::endl;
//     int return_code = system(pre_requisites_command.c_str());
//     return 0;
// }

#else
int main() {
    std::cout << "JSON dependency does not exist... Please execute make in package manager root directory." << std::endl;
    return 0;
}
#endif