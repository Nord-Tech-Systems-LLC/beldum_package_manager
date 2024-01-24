#pragma once

#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

enum class PossibleOptions {
    NONE,
    INIT,
    INSTALL,
    UNINSTALL,
    CLEAN,
    HELP,
    VERSION,
    LIST_PACKAGES
};

class Package {
   public:
    std::string name;
    std::string version;
    std::string description;
};

class PackageManager {
   public:
    static PackageManager& getInstance() {
        static PackageManager instance;
        return instance;
    }

    PossibleOptions parse_arguments(int argc, char* argv[]);
    void check_passed_shell_arguments(PossibleOptions options);
    bool file_exists(const std::string& name);

   private:
    PackageManager(){};
    // ~PackageManager();

    bool show_warning();
    void print_help();
};
#endif