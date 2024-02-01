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
 * - functionality to install all packages
 * - package version management
 * - add ability to add packages via the terminal beldum --add [package_name] [git_link]
 *
 */

// #define JSON_DEPENDENCY_EXIST # for vscode prettier, comment out when not coding

// libraries
#include "headerfiles/msql_connection.hpp"
#include "headerfiles/package_manager.hpp"

#ifndef JSON_DEPENDENCY_EXIST
#error("JSON_DEPENDENCY_EXIST not defined");
#endif

#include "json/single_include/nlohmann/json.hpp"

const char *hostname = "localhost";
const char *username = "vikingofvalhalla";
// set the password for mysql server here
const char *password = "************"; /* set me first */
const char *database = "beldum_package_manager";
const char *statement = "select * from packages";

int main(int argc, char *argv[]) {
    PackageManager instance = PackageManager::getInstance();
    PossibleOptions options = instance.parse_arguments(argc, argv);

    // TODO: SQL connection for when ready to use database
    // MySQLConnection sql_instance = MySQLConnection::getInstance();
    // sql_instance.connectToDatabase(hostname,
    //                                database,
    //                                username,
    //                                password);
    // sql_instance.executeQuery(statement);

    // checks file dependencies
    if (options == PossibleOptions::INIT || options == PossibleOptions::HELP || options == PossibleOptions::VERSION) {
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
