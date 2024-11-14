
#include "headerfiles/package_manager.hpp"
#include "headerfiles/beldum_clean.hpp"
#include "headerfiles/beldum_init.hpp"
#include "headerfiles/beldum_install.hpp"
#include "headerfiles/beldum_list.hpp"
#include "headerfiles/beldum_uninstall.hpp"
#include "headerfiles/global_utilities.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

// #define JSON_DEPENDENCY_EXIST # for vscode prettier, comment out when not coding

#ifndef JSON_DEPENDENCY_EXIST
#error("JSON_DEPENDENCY_EXIST not defined");
#endif
#include "nlohmann/json.hpp"

#ifndef FMT_DEPENDENCY_EXIST
#error("FMT_DEPENDENCY_EXIST not defined");
#endif
#include "fmt/core.h"

#ifndef CLI11_DEPENDENCY_EXIST
#error("CLI11_DEPENDENCY_EXIST not defined");
#endif
#include "CLI/CLI.hpp"

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "unknown" // Fallback in case it’s not defined
#endif

Package individual_package;

int PackageManager::check_passed_shell_arguments(PossibleOptions options) {
    logger.log("Starting argument check with option: " + std::to_string(static_cast<int>(options)));
    // read shell arguments passed
    int return_code = 0;
    std::string command;
    std::string repository_URL;
    std::string repo_name;
    std::string repo_version;
    std::string repo_type;

    logger.log("Available Packages Path: " + available_packages_path);

    // instantiate json object
    using json = nlohmann::json;
    json package_data;
    json installed_data;

    // Extract requested package details
    std::string requested_package = individual_package.name;
    logger.log("Requested package to process: " + requested_package);

    // Get and log the current path
    std::string testing = std::string(std::filesystem::current_path());
    logger.log("Current directory path: " + testing);

    std::string result_string;
    switch (options) {
    case PossibleOptions::VERSION:
        // git version number
        logger.log("Displaying version information.");
        repo_version = PROJECT_VERSION;
        fmt::print("\nBeldum Version: {}\n\n", repo_version);

        return return_code;

    case PossibleOptions::INIT:
        return beldum_init(installed_packages_path, available_packages_path);

    case PossibleOptions::INSTALL:
        return beldum_install(requested_package,
                              repo_name,
                              repo_version,
                              repository_URL,
                              repo_type,
                              command,
                              available_packages_path,
                              installed_packages_path,
                              cmake_list_path,
                              cmakeStaticCommand,
                              cmakeHeaderOnlyCommand);

    case PossibleOptions::UNINSTALL:
        return beldum_uninstall(requested_package,
                                repo_name,
                                repo_type,
                                command,
                                installed_packages_path,
                                cmake_list_path,
                                cmakeStaticCommand,
                                cmakeHeaderOnlyCommand);

    case PossibleOptions::LIST_INSTALLED_PACKAGES:
        return beldum_list_installed(installed_packages_path);

    case PossibleOptions::LIST_AVAILABLE_PACKAGES:
        return beldum_list_available(available_packages_path);

    case PossibleOptions::CLEAN:
        return beldum_clean(command, installed_packages_path);
    }
    return return_code;
}

// function to parse command line arguments and assign them to the enum
int PackageManager::parse_arguments(int argc, char **argv) {
    CLI::App app{"Beldum Package Manager"};

    app.footer("\n");

    // Define subcommands without any dependencies
    auto init_cmd = app.add_subcommand("init", "Initialize the project");
    init_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::INIT); });

    auto version_cmd = app.add_subcommand("version", "Show version information");
    version_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::VERSION); });

    // Define subcommands with file dependencies
    std::string package_name;

    auto install_cmd = app.add_subcommand("install", "Install a dependency");
    install_cmd->add_option("package_name", package_name, "Name of the package to install")
        ->required();
    install_cmd->callback([this, &package_name]() {
        if (!file_exists(installed_packages_path) || !file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or "
                         "available_packages.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::INSTALL);
    });

    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstall a dependency");
    uninstall_cmd->add_option("package_name", package_name, "Name of the package to uninstall")
        ->required();
    uninstall_cmd->callback([this, &package_name]() {
        if (!file_exists(installed_packages_path) || !file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or "
                         "available_packages.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::UNINSTALL);
    });

    auto list_cmd = app.add_subcommand("list", "List installed packages");

    // Define flags for installed and available options
    bool list_installed = false;
    bool list_available = false;
    list_cmd->add_flag("--installed", list_installed, "List installed packages");
    list_cmd->add_flag("--available", list_available, "List available packages");

    list_cmd->callback([this, &list_installed, &list_available]() {
        if (!file_exists(installed_packages_path) || !file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or "
                         "available_packages.json). Please run 'beldum init' first.\n";
            return;
        }

        if (list_installed) {
            check_passed_shell_arguments(PossibleOptions::LIST_INSTALLED_PACKAGES);
        } else if (list_available) {
            check_passed_shell_arguments(PossibleOptions::LIST_AVAILABLE_PACKAGES);
        } else {
            std::cerr << "Error: Please specify either --installed or --available.\n";
        }
    });

    auto clean_cmd = app.add_subcommand("clean", "Clean build directory and dependencies");
    clean_cmd->callback([this]() {
        if (!file_exists(installed_packages_path) || !file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or "
                         "available_packages.json). Please run 'beldum init' first.\n";
            return;
        }
        check_passed_shell_arguments(PossibleOptions::CLEAN);
    });

    try {
        CLI11_PARSE(app, argc, argv);
        // Create a stringstream to accumulate the output
        std::stringstream ss;

        // Add the argument count to the stringstream
        ss << "count=" << argc << ", ";

        // Add each argument to the stringstream
        ss << "arguments: \"";
        for (int i = 0; i < argc; ++i) {
            if (i < argc - 1) {
                ss << argv[i] << " ";
            } else {
                ss << argv[i];
            }
        }
        logger.log("Command line arguments: " + ss.str() + "\" parsed successfully.");
        return 0;
    } catch (const CLI::ParseError &e) {
        logger.logError("Error while parsing command line arguments: " + std::string(e.what()));
        return e.get_exit_code();
    }

    return 0;
}
