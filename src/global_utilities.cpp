

#include "headerfiles/global_utilities.hpp"
#include "fmt/core.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * Executes command and returns result
 * @param command
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

    // check user response
    if (response == "y" || response == "Y") {
        fmt::print("Proceeding...\n");
        return true;
    } else {
        fmt::print("Operation canceled.\n");
        return false;
    }
}

bool file_exists(const std::string &name) {
    return std::filesystem::exists(name.c_str());
}