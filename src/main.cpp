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
 * - create package update
 * - create package remove
 * - when creating package change the logic for where the cpp_libs file is saved
 * - need to move folder structure to separate backend / frontend
 * - need to figure out where to move cpp_libs so it's based on the current directory and not inside the package manager
 * - setup table with commands
 * - create function list of all currently installed packages
 *
 */

// #define JSON_DEPENDENCY_EXIST # for vscode prettier, comment out when not coding

// libraries
#include "headerfiles/package_manager.hpp"

#ifdef JSON_DEPENDENCY_EXIST
#include "json/single_include/nlohmann/json.hpp"

int main(int argc, char* argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);

    // checks file dependencies
    if (options == PossibleOptions::INIT) {
        // if initializing project
        instance.check_passed_shell_arguments(options);
        return 0;
    } else if (!instance.file_exists("installed_packages.json") || !instance.file_exists("package.json")) {
        // if installed_packages.json or package.json doesn't exist
        std::cerr << "\nYou're missing the installed_packages.json or package.json, please run --init.\n"
                  << std::endl;
        return 1;
    } else {
        // else anything else
        instance.check_passed_shell_arguments(options);
    }

    return 0;
}

#else
int main() {
    std::cout << "\nJSON dependency does not exist... Please execute make in package manager root directory." << std::endl;
    return 0;
}
#endif