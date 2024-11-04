
/**
 * TODO:
 * - create package update
 * - functionality to install all packages
 * - package version management
 * - package manager version management
 * - add ability to add packages via the terminal beldum --add [package_name] [git_link]
 *
 */

// libraries
#include "headerfiles/package_manager.hpp"

int main(int argc, char **argv)
{
    PackageManager instance = PackageManager::getInstance();
    instance.parse_arguments(argc, argv);

    return 0;
}
