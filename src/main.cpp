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
 * - functionality to install all packages
 * - package version management
 * - package manager version management
 *
 */

// #define JSON_DEPENDENCY_EXIST # for vscode prettier, comment out when not coding

// libraries
#include "headerfiles/package_manager.hpp"
#include "headerfiles/beldum_init.hpp"

#ifndef JSON_DEPENDENCY_EXIST
#error("JSON_DEPENDENCY_EXIST not defined");
#endif

#include "nlohmann/json.hpp"

int main(int argc, char *argv[])
{
    PackageManager instance = PackageManager::getInstance();
    BeldumInit beldum;
    PossibleOptions options = instance.parse_arguments(argc, argv);

    // checks file dependencies
<<<<<<< Updated upstream
    if (options == PossibleOptions::INIT || options == PossibleOptions::HELP) {
=======
    if (options == PossibleOptions::INIT || options == PossibleOptions::HELP || options == PossibleOptions::VERSION)
    {
>>>>>>> Stashed changes
        // if initializing project
        instance.check_passed_shell_arguments(options);
        return 0;
    }
    else if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists("package.json"))
    {
        // if installed_packages.json or package.json doesn't exist
        std::cerr << "\nYou're missing the installed_packages.json or package.json, please run --init.\n"
                  << std::endl;
        return 1;
    }
    else
    {
        // else anything else
        instance.check_passed_shell_arguments(options);
    }

    return 0;
}
