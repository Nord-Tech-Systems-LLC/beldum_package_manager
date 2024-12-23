
#include "header_files/package_manager.hpp"
#include "header_files/beldum_install.hpp"
#include "header_files/beldum_list.hpp"
#include "header_files/beldum_run.hpp"
#include "header_files/beldum_setup.hpp"
#include "header_files/beldum_uninstall.hpp"
#include "header_files/global_utilities.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

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
#define PROJECT_VERSION "unknown" // fallback in case version isn't defined
#endif

Package individual_package;

int PackageManager::check_passed_shell_arguments(PossibleOptions options) {
    logger.log("Starting argument check with option: " + std::to_string(static_cast<int>(options)));
    // read shell arguments passed
    int return_code = 0;
    std::string repo_version;

    // instantiate json object
    using json = nlohmann::ordered_json;
    json package_data;
    json installed_data;

    std::string requested_package = individual_package.name; // extract requested package details
    std::string single_package_directory_path =
        fmt::format("{}{}.json", available_packages_path, requested_package);
    std::string current_path = std::string(std::filesystem::current_path()); // get the current path

    logger.log("Requested package to process: " + requested_package);
    logger.log("Current directory path: " + current_path);

    std::string result_string;
    switch (options) {
    case PossibleOptions::VERSION:
        // git version number
        logger.log("Displaying version information.");
        repo_version = PROJECT_VERSION;
        fmt::print("\nBeldum Version: {}\n\n", repo_version);

        return return_code;

    case PossibleOptions::CREATE:
        return beldum_create_project(project_name);

    case PossibleOptions::INIT:
        return beldum_init(project_name);

    case PossibleOptions::INSTALL_ALL:
        return beldum_install_all();

    case PossibleOptions::INSTALL_SINGLE:
        return beldum_install_single(
            requested_package, repo_version, single_package_directory_path);

    case PossibleOptions::RUN:
        return execute_build_script(script_name);

    case PossibleOptions::UNINSTALL:
        return beldum_uninstall(requested_package, single_package_directory_path);

    case PossibleOptions::LIST_INSTALLED_PACKAGES:
        return beldum_list_installed();

    case PossibleOptions::LIST_AVAILABLE_PACKAGES:
        return beldum_list_available();
    }
    return return_code;
}

// function to parse command line arguments and assign them to the enum
int PackageManager::parse_arguments(int argc, char **argv) {
    CLI::App app{"Beldum Package Manager"};

    app.footer("\n");

    // initialize new project
    auto init_cmd = app.add_subcommand("init", "Initialize new Beldum project");
    init_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::INIT); });

    // create new project
    auto create_cmd = app.add_subcommand("create", "Create new Beldum project");
    create_cmd->add_option("project_name", project_name, "Create a new project");
    create_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::CREATE); });

    // display version of beldum
    auto version_cmd = app.add_subcommand("version", "Show version information");
    version_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::VERSION); });

    // Define subcommands with file dependencies
    std::string package_name;

    // install library
    auto install_cmd = app.add_subcommand("install", "Install a dependency");
    install_cmd->add_option("package_name", package_name, "Name of the package to install");
    install_cmd->callback([this, &package_name]() {
        if (package_name.empty()) {
            // install all packages
            check_passed_shell_arguments(PossibleOptions::INSTALL_ALL);
        } else {
            // install a single package
            if (!file_exists(beldum_json_path) || !file_exists(available_packages_path)) {
                fmt::print("Error: Missing required files ({} or {}). Please run "
                           "'beldum init' first.\n",
                           beldum_json_path,
                           available_packages_path);
                return;
            }
            individual_package.name = package_name;
            check_passed_shell_arguments(PossibleOptions::INSTALL_SINGLE);
        }
    });

    // uninstall library
    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstall a dependency");
    uninstall_cmd->add_option("package_name", package_name, "Name of the package to uninstall")
        ->required();
    uninstall_cmd->callback([this, &package_name]() {
        if (!file_exists(beldum_json_path) || !file_exists(available_packages_path)) {
            fmt::print("Error: Missing required files ({} or {}). Please run "
                       "'beldum init' first.\n",
                       beldum_json_path,
                       available_packages_path);
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::UNINSTALL);
    });

    // list available and installed packages
    auto list_cmd = app.add_subcommand("list", "List installed packages");
    bool list_installed = false; // flag for installed
    bool list_available = false; // flag for available
    list_cmd->add_flag("--installed", list_installed, "List installed packages");
    list_cmd->add_flag("--available", list_available, "List available packages");
    list_cmd->callback([this, &list_installed, &list_available]() {
        if (!file_exists(beldum_json_path) || !file_exists(available_packages_path)) {
            fmt::print("Error: Missing required files ({} or {}). Please run "
                       "'beldum init' first.\n",
                       beldum_json_path,
                       available_packages_path);
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

    // beldum command to run scripts
    auto run_cmd = app.add_subcommand("run", "Run Beldum script");
    run_cmd->add_option("script_name", script_name, "Run Beldum script");
    run_cmd->callback([this]() { check_passed_shell_arguments(PossibleOptions::RUN); });

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
