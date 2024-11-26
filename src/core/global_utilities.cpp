

#include "header_files/global_utilities.hpp"
#include "fmt/core.h"

#include <array>
#include <cstdlib> // For getenv
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * GLOBAL VARIABLES THROUGHOUT PACKAGE MANAGER
 */
std::string beldum_json_path = "beldum.json";
std::string available_packages_path = std::string(getenv("HOME")) + "/.beldum/packages/";

/**
 * Executes command and returns result
 * @param command = command to be executed
 */
std::string execute_command(std::string cmd) {
    // Define the specific function pointer type for the deleter
    using PipeDeleter = int (*)(FILE *);

    // Pipe command result to string for use
    std::array<char, 1024> buffer;
    std::string result;

    // Initialize pipe with popen and custom deleter
    std::unique_ptr<FILE, PipeDeleter> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // Read the output from the command
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

/**
 * Shows warning and cancels if N is selected
 */
bool show_warning() {
    // display the warning message
    fmt::print("WARNING: This operation may have consequences.\n");
    fmt::print("Do you want to proceed? (y/n)");

    // get user input
    std::string response;
    std::getline(std::cin, response);

    // verification
    fmt::print("You entered: {}\n", response);
    if (response.length() > 1) {
        fmt::print("You entered too many characters, please enter either 'y' or 'n'.\n");
        return false;
    }

    // check user response
    if (response == "y" || response == "Y") {
        fmt::print("Proceeding...\n");
        return true;
    } else {
        fmt::print("Operation canceled.\n");
        return false;
    }
}

/**
 * Returns boolean if directory / path exists
 * @param name = name of path
 */
bool file_exists(const std::string &name) {
    return std::filesystem::exists(name.c_str());
}

/**
 * Returns if str ends with suffix
 * @param str = string to check if contains suffix
 * @param suffix = suffix (ends with)
 */
bool ends_with(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/**
 * Returns if str starts with prefix
 * @param str = string to check if contains prefix
 * @param prefix = prefix (starts with)
 */
bool starts_with(std::string_view str, std::string_view prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}