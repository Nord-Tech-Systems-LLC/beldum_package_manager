#include "headerfiles/package_manager.hpp"
#include "headerfiles/beldum_init.hpp"
#include "beldum_install.cpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
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

int PackageManager::check_passed_shell_arguments(PossibleOptions options)
{
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
    switch (options)
    {
    case PossibleOptions::VERSION:
        // git version number
        logger.log("Displaying version information.");
        repo_version = PROJECT_VERSION;
        fmt::print("\nBeldum Version: {}\n\n", repo_version);

        return return_code;
    /**
     * INIT ACTIONS
     */
    case PossibleOptions::INIT:
        // creates packages folder if it doesn't exist
        std::cout << "\n";
        if (beldum.file_exists(installed_packages_path) && beldum.file_exists(available_packages_path))
        {
            fmt::print("Available_packages.json and installed_packages.json already exist.\nTry installing an example package with --install example_package\n\n");
            return_code = 1;
            return return_code;
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
        return return_code;

    /**
     * INSTALL ACTIONS
     */
    case PossibleOptions::INSTALL:
        

        return beldum_install(requested_package, available_packages_path, repo_name, repo_version, command, installed_packages_path);

    /**
     * LIST INSTALLED / AVAILABLE PACKAGES
     */
    case PossibleOptions::LIST_INSTALLED_PACKAGES:

        if (beldum.file_exists("installed_packages.json"))
        {
            installed_packages_file.open(installed_packages_path);
            if (!installed_packages_file.is_open())
            {
                logger.logError("Error: Failed to open installed_packages.json file.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened installed_packages.json file.");
            installed_data = json::parse(installed_packages_file);
            installed_packages_file.close();

            // Print a header with better alignment and a divider
            fmt::print("\n{:<40} {:<20}\n", "PACKAGE", "VERSION");
            fmt::print("{:<40} {:<20}\n", std::string(40, '-'), std::string(20, '-'));

            // Iterate over installed packages and display each one
            for (const auto &package : installed_data["packages"])
            {
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
            return_code = 1;
            return return_code;
        }
        return return_code;

    case PossibleOptions::LIST_AVAILABLE_PACKAGES:

        // Check if the available packages file exists
        if (beldum.file_exists(available_packages_path))
        {
            packages_file.open(available_packages_path);
            if (!packages_file.is_open())
            {
                logger.logError("Error: Failed to open available_packages.json file.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened available_packages.json file.");
            package_data = json::parse(packages_file);
            packages_file.close();

            // Print header with alignment and divider
            fmt::print("\n{:<20} {:<40} {:<60}\n", "PACKAGE", "DESCRIPTION", "REPOSITORY URL");
            fmt::print("{:<20} {:<40} {:<60}\n", std::string(20, '-'), std::string(40, '-'), std::string(60, '-'));

            // Iterate over each package in the JSON data
            for (const auto &package_entry : package_data["packages"].items())
            {
                const std::string &package_name = package_entry.key();
                const json &package = package_entry.value();

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
            return_code = 1;
            return return_code;
        }

        return return_code;

    case PossibleOptions::UNINSTALL:
        installed_packages_file.open(installed_packages_path);
        if (!installed_packages_file.is_open())
        {
            logger.logError("Error: Failed to open installed_packages.json file.");
            return_code = 1;
            return return_code;
        }
        logger.log("Opened installed_packages.json file.");
        installed_data = json::parse(installed_packages_file);
        installed_packages_file.close();

        if (installed_data["packages"].contains(requested_package))
        {
            logger.log("Uninstalling package: " + requested_package);
            repo_name = requested_package;
            repo_type = installed_data["packages"][requested_package]["repo_type"];

            // Remove the repo
            command = "rm -rf target/debug/deps/" + std::string(requested_package);
            logger.log("Executing command: " + command);
            return_code = system(command.c_str());

            output.open(installed_packages_path);
            if (!output.is_open())
            {
                logger.logError("Error: Failed to open installed_packages.json file.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened installed_packages.json file.");

            // Update installed_packages.json
            installed_data["packages"].erase(requested_package);

            output << installed_data.dump(4);
            output.close();

            /**
             * Read the file line by line
             */
            cmake_list_file.open(cmake_list_path);
            if (!cmake_list_file.is_open())
            {
                logger.logError("Error: Failed to open CMakeLists.txt file for reading.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened CMakeLists.txt file for reading.");

            // commands for adding
            cmakeStaticCommand = fmt::format("add_subdirectory(${{BELDUM_LIB_DIR}}/{})", repo_name);
            cmakeHeaderOnlyCommand = fmt::format("include_directories(${{BELDUM_LIB_DIR}}/{})", repo_name);

            //   findAndInsert(cmakeLines, "BELDUM-STATIC-ONLY", cmakeStaticCommand, repo_name);
            while (std::getline(cmake_list_file, cmakeLine))
            {
                cmakeLines.push_back(cmakeLine); // Add the current line to the vector
            }

            /**
             * Logic for when / where to insert in CMakeLists.txt
             */

            if (repo_type == "header-only")
            {
                // if beldum header config is found in CMakeLists.txt
                for (std::string &sentence : cmakeLines)
                {
                    if (sentence.find(cmakeHeaderOnlyCommand) != std::string::npos)
                    {
                        // Erase after the current sentence in the vector
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        cmakeLines.erase(it); // Erase the found sentence
                        break;
                    }
                }
            }

            if (repo_type == "static")
            {
                // if beldum static command config is found in CMakeLists.txt
                for (std::string &sentence : cmakeLines)
                {
                    if (sentence.find(cmakeStaticCommand) != std::string::npos)
                    {
                        // Erase after the current sentence in the vector
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        cmakeLines.erase(it); // Erase the found sentence
                        break;
                    }
                }
                // if beldum linker already exists, add to linker
                for (std::string &sentence : cmakeLines)
                {
                    if (sentence.find("    " + repo_name + "::" + repo_name) != std::string::npos)
                    {
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        cmakeLines.erase(it);
                    }
                }
                // if beldum linker MY_LIBRARIES variable
                for (std::string &sentence : cmakeLines)
                {
                    if (sentence.find("# BELDUM-LINKER") != std::string::npos)
                    {
                        auto it = std::find(cmakeLines.begin(), cmakeLines.end(), sentence);
                        // if a library does not exist in the MY_LIBRARIES variable then erase the next three lines
                        if (it + 2 < cmakeLines.end() && (it + 2)->find("::") == std::string::npos)
                        {
                            cmakeLines.erase(it);
                            cmakeLines.erase(it);
                            cmakeLines.erase(it);
                        }
                    }
                }
            }
            cmake_list_file.close();

            /**
             * Write to CMakeLists.txt
             */
            output.open(cmake_list_path);
            if (!output.is_open())
            {
                logger.logError("Error: Failed to open CMakeLists.txt file for writing.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened CMakeLists.txt file for writing.");
            for (const auto &outputLine : cmakeLines)
            {
                output << outputLine << '\n';
            }
            output.close();

            logger.log("Package " + requested_package + " successfully uninstalled.");
            fmt::print("\nPackage {} successfully uninstalled.\n\n", requested_package);
        }
        else
        {
            logger.logError("Attempted to uninstall package: " + requested_package + ", but it was not found.");
            std::cout << "\n\nPackage " << requested_package << " does not exist. Please check the installed packages and try again...\n\n"
                      << std::endl;
            return_code = 1;
            return return_code;
        }
        return return_code;

    /**
     * CLEAN ACTIONS
     */
    case PossibleOptions::CLEAN:
        // using bash to clean the library folder
        if (show_warning())
        {
            command = "sudo find target/debug/deps/* -maxdepth 0 -type d ! -name \"json\" -exec rm -r {} +";

            output.open(installed_packages_path);
            if (!output.is_open())
            {
                logger.logError("Error: Failed to open installed_packages.json file.");
                return_code = 1;
                return return_code;
            }
            logger.log("Opened installed_packages.json file.");
            installed_data["packages"] = {};
            output << installed_data;

            return_code = system(command.c_str());
            fmt::print("\nSuccessfully cleaned project directory.\n");
            std::cout << std::endl;
        }
        else
        {
            return_code = 1;
            return return_code;
        };
    }
    return return_code;
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
        if (!beldum.file_exists(installed_packages_path) || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or available_packages.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::INSTALL); });

    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstall a dependency");
    uninstall_cmd->add_option("package_name", package_name, "Name of the package to uninstall")->required();
    uninstall_cmd->callback([this, &package_name]()
                            {
        if (!beldum.file_exists(installed_packages_path) || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or available_packages.json). Please run 'beldum init' first.\n";
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
        if (!beldum.file_exists(installed_packages_path) || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or available_packages.json). Please run 'beldum init' first.\n";
            return;
        }

        if (list_installed) {
            check_passed_shell_arguments(PossibleOptions::LIST_INSTALLED_PACKAGES);
        } else if (list_available) {
            check_passed_shell_arguments(PossibleOptions::LIST_AVAILABLE_PACKAGES);
        } else {
            std::cerr << "Error: Please specify either --installed or --available.\n";
        } });

    auto clean_cmd = app.add_subcommand("clean", "Clean build directory and dependencies");
    clean_cmd->callback([this]()
                        {
        if (!beldum.file_exists(installed_packages_path) || !beldum.file_exists(available_packages_path)) {
            std::cerr << "Error: Missing required files (installed_packages.json or available_packages.json). Please run 'beldum init' first.\n";
            return;
        }
        check_passed_shell_arguments(PossibleOptions::CLEAN); });

    try
    {
        CLI11_PARSE(app, argc, argv);
        // Create a stringstream to accumulate the output
        std::stringstream ss;

        // Add the argument count to the stringstream
        ss << "count=" << argc << ", ";

        // Add each argument to the stringstream
        ss << "arguments: \"";
        for (int i = 0; i < argc; ++i)
        {
            if (i < argc - 1)
            {
                ss << argv[i] << " ";
            }
            else
            {
                ss << argv[i];
            }
        }
        logger.log("Command line arguments: " + ss.str() + "\" parsed successfully.");
        return 0;
    }
    catch (const CLI::ParseError &e)
    {
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
