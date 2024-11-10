#pragma once

#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <cstring>
#include <cstdlib> // For getenv
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <vector>
#include "beldum_init.hpp"
#include "beldum_logging.hpp"

enum class PossibleOptions
{
    INIT,
    INSTALL,
    UNINSTALL,
    CLEAN,
    VERSION,
    LIST_INSTALLED_PACKAGES,
    LIST_AVAILABLE_PACKAGES
};

class Package
{
public:
    std::string name;
    std::string version;
    std::string description;
};

class PackageManager
{
public:
    static PackageManager &getInstance()
    {
        static PackageManager instance;
        return instance;
    }

    int parse_arguments(int argc, char **argv);
    void check_passed_shell_arguments(PossibleOptions options);

private:
    PackageManager() : logger() { 
        // logger.log("PackageManager initialized."); 
    }

    // Delete the copy constructor and copy assignment operator
    PackageManager(const PackageManager&) = delete;
    PackageManager& operator=(const PackageManager&) = delete;

    const std::string available_packages_path = std::string(getenv("HOME")) + "/.beldum/packages/available_packages.json";
    std::ifstream packages_file;

    const std::string installed_packages_path = "installed_packages.json";
    std::ifstream installed_packages_file;

    std::ofstream output;

    BeldumLogging logger;
    BeldumInit beldum;

    bool show_warning();

};
#endif
