
/**
 * TODO:
 * - create package update
 * - mysql backend with packages
 * - package version management
 * - add ability to add packages via the terminal beldum --add [package_name] [git_link]
 *
 */

// libraries
#include "header_files/package_manager.hpp"

int main(int argc, char **argv) {
    PackageManager &instance = PackageManager::getInstance();
    instance.parse_arguments(argc, argv);

    return 0;
}
