#pragma once

#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include "beldum_init.hpp"

enum class PossibleOptions
{
    NONE,
    INIT,
    INSTALL,
    UNINSTALL,
    CLEAN,
    HELP,
    VERSION,
    LIST_PACKAGES
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
    BeldumInit beldum;
    void check_passed_shell_arguments(PossibleOptions options);

private:
    PackageManager() {};
    // ~PackageManager();

    bool show_warning();
    void print_help();
};
#endif
