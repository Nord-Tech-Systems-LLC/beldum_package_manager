#pragma once

#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

enum class PossibleOptions {
    NONE,
    INSTALL,
    CLEAN,
    HELP,
    VERSION,
    INPUT_FILE,
    OUTPUT_FILE
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
    private:
        PackageManager(){};
        // ~PackageManager();
        
        bool show_warning();
        void print_table(const std::vector<Package>& package);


};
#endif