#include "headerfiles/package_manager.hpp"
#include "headerfiles/beldum_init.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

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
#define PROJECT_VERSION "unknown"  // Fallback in case it’s not defined
#endif

Package individual_package;

std::string exec(const char *cmd)
{
    // pipes command result to string for use
    std::array<char, 1024> buffer;
    std::string result;
    std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(cmd, "r"), (int (*)(FILE *))pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

void PackageManager::check_passed_shell_arguments(PossibleOptions options)
{
    logger.log("Starting argument check with option: " + std::to_string(static_cast<int>(options)));
    // read shell arguments passed
    int return_code;
    std::string command;
    std::string repository_URL;
    std::string repo_name;
    std::string repo_version;
    

    logger.log("Available Packages Path: " + available_packages_path);
    // Open package files -- gets list of packages
    std::ifstream packages_file(available_packages_path);
    if (!packages_file.is_open()) {
        logger.logError("Error: Failed to open available_packages.json file.");
        return;
    }
    logger.log("Opened available_packages.json file.");

    std::ifstream installed_packages("installed_packages.json");
    // if (!installed_packages.is_open()) {
    //     logger.logError("Error: Failed to open installed_packages.json file.");
    //     return;
    // }
    // logger.log("Opened installed_packages.json file.");

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
    switch (options)
    {
    case PossibleOptions::VERSION:
        // git version number
        logger.log("Displaying version information.");
        repo_version = PROJECT_VERSION;
        fmt::print("\nBeldum Version: {}\n\n", repo_version);

        break;
    /**
     * INIT ACTIONS
     */
    case PossibleOptions::INIT:
        // creates packages folder if it doesn't exist
        std::cout << "\n";
        if (beldum.file_exists("installed_packages.json") && beldum.file_exists(available_packages_path))
        {
            fmt::print("Package.json and installed_packages.json already exist.\nTry installing an example package with --install example_package\n\n");
            break;
        }
        else
        {
            beldum.create_installed_packages(installed_data);
            // beldum.create_package_json(package_data);
            beldum.create_build_script();
            beldum.create_src_and_main();
            beldum.create_cmake_lists();
        }

        std::cout << "\n";
        return_code = 0;
        break;

    /**
     * INSTALL ACTIONS
     */
    case PossibleOptions::INSTALL:

        logger.log("Attempting to install package: " + requested_package);

        // Parse package.json and installed_packages.json
        try {
            package_data = json::parse(packages_file);
            fmt::print("Package Data: {}", package_data.dump(4));
            installed_data = json::parse(installed_packages);
            logger.log("Parsed JSON data from package files.");
        } catch (const json::parse_error &e) {
            logger.logError("Failed to parse JSON data: " + std::string(e.what()));
            break;
        }

        // Check if package is already installed
        if (installed_data["packages"].contains(requested_package)) {
            logger.logWarning("The package \"" + requested_package + "\" is already installed.");
            fmt::print("\nThe package \"{}\" is already installed...\n\n", requested_package);
            break;
        }

        // checks if package exists in package.json
        if (package_data["packages"].contains(requested_package))
        {
            repo_name = requested_package;
            repository_URL = package_data["packages"][requested_package]["repository_url"];
            logger.log("Found package \"" + repo_name + "\" in package.json with repository URL: " + repository_URL);

            // strips quotes from directory
            for (char c : testing)
            {
                if (c != '\"')
                {
                    result_string += c;
                }
            }
            logger.log("Cleaned directory path: " + result_string);

            // clone the repo
            command = fmt::format("cd {} && git clone {} target/debug/deps/{}", result_string, std::string(repository_URL), std::string(repo_name));
            logger.log("Executing command to clone repository: " + command);
            return_code = system(command.c_str());

            // git version number
            command = fmt::format("cd {}/target/debug/deps/{} && git describe --tags --abbrev=0", result_string, std::string(repo_name));
            repo_version = exec(command.c_str());
            repo_version.erase(std::remove(repo_version.begin(), repo_version.end(), '\n'), repo_version.end()); // removes new line character from version

            // assigning unknown if no version number exists in git repo
            if (repo_version.empty())
            {
                repo_version = "unknown";
                logger.logWarning("No version tag found in repository. Defaulting to version: unknown.");
            } else {
                logger.log("Retrieved version for " + repo_name + ": " + repo_version);
            }
            // Update installed package manager
            try {
                std::ofstream output("installed_packages.json");
                installed_data["packages"][repo_name] = {
                    {"git_link", repository_URL},
                    {"repo_name", repo_name},
                    {"version", repo_version},
                };
                output << installed_data.dump(4);
                logger.log("Successfully updated installed_packages.json with " + repo_name);
            } catch (const std::exception &e) {
                logger.logError("Failed to update installed_packages.json: " + std::string(e.what()));
            }
        }
        else
        {
            logger.logWarning("Package \"" + requested_package + "\" does not exist in package.json.");
            fmt::print("\nPackage \"{}\" does not exist. Please enter the correct package name.\n\n", requested_package);
        }

        break;

    /**
     * LIST INSTALLED / AVAILABLE PACKAGES
     */
    case PossibleOptions::LIST_INSTALLED_PACKAGES:

        if (beldum.file_exists("installed_packages.json"))
        {
            installed_data = json::parse(installed_packages);

            // Print a header with better alignment and a divider
            fmt::print("\n{:<40} {:<20}\n", "PACKAGE", "VERSION");
            fmt::print("{:<40} {:<20}\n", std::string(40, '-'), std::string(20, '-'));

            // Iterate over installed packages and display each one
            for (const auto& package : installed_data["packages"]) {
                // std::string repo_name = package.value()["repo_name"];
                // std::string version = package.value()["version"];
                fmt::print("{:<40} {:<20}\n", std::string(package["repo_name"]), std::string(package["version"]));
            }

            // Bottom spacing
            fmt::print("\n\n");
        }
        else
        {
            std::cerr << "\nThe installed_packages.json does not exist, please run --init" << std::endl;
        }
        break;

    case PossibleOptions::LIST_AVAILABLE_PACKAGES:

        // Check if the available packages file exists
        if (beldum.file_exists(available_packages_path)) 
        {
            package_data = json::parse(packages_file);

            // Print header with alignment and divider
            fmt::print("\n{:<20} {:<40} {:<60}\n", "PACKAGE", "DESCRIPTION", "REPOSITORY URL");
            fmt::print("{:<20} {:<40} {:<60}\n", std::string(20, '-'), std::string(40, '-'), std::string(60, '-'));

            // Iterate over each package in the JSON data
            for (const auto& package_entry : package_data["packages"].items()) 
            {
                const std::string& package_name = package_entry.key();
                const json& package = package_entry.value();

                // Retrieve package information -- print package details with alignment
                fmt::print("{:<20} {:<40} {:<60}\n", package_name, 
                        std::string(package["description"]), 
                        std::string(package["repository_url"]));

                // // Print tags as a comma-separated list, indented
                // if (package.contains("tags")) {
                //     fmt::print("  Tags: {}\n", fmt::join(package["tags"].begin(), package["tags"].end(), ", "));
                // }
            }

            // Add spacing after the output
            fmt::print("\n\n");
        }
        else 
        {
            // Display error if the file is missing
            std::cerr << "\nThe available_packages.json does not exist. Please ensure the file is available at ~/.beldum/packages/\n";
        }
        break;
    

    case PossibleOptions::UNINSTALL:
        installed_data = json::parse(installed_packages);
        
        if (installed_data["packages"].contains(requested_package)) {
            logger.log("Uninstalling package: " + requested_package);

            // Remove the repo
            command = "rm -rf target/debug/deps/" + std::string(requested_package);
            logger.log("Executing command: " + command);
            return_code = system(command.c_str());

            // Update installed_packages.json
            installed_data["packages"].erase(requested_package);
            std::ofstream output("installed_packages.json");
            output << installed_data.dump(4);

            logger.log("Package " + requested_package + " successfully uninstalled.");
            fmt::print("\nPackage {} successfully uninstalled.\n\n", requested_package);
        } 
        else {
            logger.logError("Attempted to uninstall package: " + requested_package + ", but it was not found.");
            std::cout << "\n\nPackage " << requested_package << " does not exist. Please check the installed packages and try again...\n\n" << std::endl;
        }
        break;


    /**
     * CLEAN ACTIONS
     */
    case PossibleOptions::CLEAN:
        // using bash to clean the library folder
        if (show_warning())
        {
            command = "sudo find target/debug/deps/* -maxdepth 0 -type d ! -name \"json\" -exec rm -r {} +";
            std::ofstream output("installed_packages.json");
            installed_data["packages"] = {};
            output << installed_data;
            return_code = system(command.c_str());
            std::cout << "\nSuccessfully cleaned project directory.\n"
                      << std::endl;
            break;
        }
        else
        {
            break;
        };
    }
}

// function to parse command line arguments and assign them to the enum
int PackageManager::parse_arguments(int argc, char **argv)
{
    CLI::App app{"Beldum Package Manager"};

    app.footer("\n");

    // Define subcommands without any dependencies
    auto init_cmd = app.add_subcommand("init", "Initialize the project");
    init_cmd->callback([this]()
                       { check_passed_shell_arguments(PossibleOptions::INIT); });

    auto version_cmd = app.add_subcommand("version", "Show version information");
    version_cmd->callback([this]()
                          { check_passed_shell_arguments(PossibleOptions::VERSION); });

    // Define subcommands with file dependencies
    std::string package_name;

    auto install_cmd = app.add_subcommand("install", "Install a dependency");
    install_cmd->add_option("package_name", package_name, "Name of the package to install")->required();
    install_cmd->callback([this, &package_name]()
                          {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::INSTALL); });

    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstall a dependency");
    uninstall_cmd->add_option("package_name", package_name, "Name of the package to uninstall")->required();
    uninstall_cmd->callback([this, &package_name]()
                            {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::UNINSTALL); });

    auto list_cmd = app.add_subcommand("list", "List installed packages");
    
    // Define flags for installed and available options
    bool list_installed = false;
    bool list_available = false;
    list_cmd->add_flag("--installed", list_installed, "List installed packages");
    list_cmd->add_flag("--available", list_available, "List available packages");


    list_cmd->callback([this, &list_installed, &list_available]()
                       {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
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
    clean_cmd->callback([this]()
                        {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        check_passed_shell_arguments(PossibleOptions::CLEAN); });

    try {
        CLI11_PARSE(app, argc, argv);
        logger.log("Command line arguments parsed successfully.");
    } catch (const CLI::ParseError &e) {
        logger.logError("Error while parsing command line arguments: " + std::string(e.what()));
        return e.get_exit_code();
    }

    return 0;
}

bool PackageManager::show_warning()
{
    // display the warning message
    fmt::print("WARNING: This operation may have consequences.\n");
    fmt::print("Do you want to proceed? (y/n)");

    // get user input
    std::string response;
    std::getline(std::cin, response);

    // verification
    fmt::print("You entered: {}\n", response);

    // check user response
    if (response == "y" || response == "Y")
    {
        fmt::print("Proceeding...\n");
        return true;
    }
    else
    {
        fmt::print("Operation canceled.\n");
        return false;
    }
}
