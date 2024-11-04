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
#error("PROJECT_VERSION not defined");
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

void create_row_help_menu(const std::string &command_flag, const std::string &description)
{
    std::cout << std::setw(20) << std::left << command_flag.c_str();
    std::cout << std::setw(60) << std::right << description.c_str() << std::endl;
}

void PackageManager::check_passed_shell_arguments(PossibleOptions options)
{
    // read shell arguments passed
    int return_code;
    std::string command;
    std::string repository_URL;
    std::string repo_name;
    std::string repo_version;

    // gets list of packages
    std::ifstream packages_file("package.json");
    std::ifstream installed_packages("installed_packages.json");

    // instantiate json object
    using json = nlohmann::json;
    json package_data;
    json installed_data;

    std::string requested_package = individual_package.name;
    std::string testing = std::string(std::filesystem::current_path());
    std::string result_string;
    switch (options)
    {
    case PossibleOptions::VERSION:
        // git version number

        // repo_version = PROJECT_VERSION;
        repo_version = "UNKNOWN";
        fmt::print("\nBeldum Version: {}\n\n", repo_version);

        break;
    /**
     * INIT ACTIONS
     */
    case PossibleOptions::INIT:
        // creates packages folder if it doesn't exist
        std::cout << "\n";
        if (beldum.file_exists("installed_packages.json") && beldum.file_exists("package.json"))
        {
            fmt::print("Package.json and installed_packages.json already exist.\nTry installing an example package with --install example_package\n\n");
            break;
        }
        else
        {
            beldum.create_installed_packages(installed_data);
            beldum.create_package_json(package_data);
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
        // parse package.json and installed_packages.json
        package_data = json::parse(packages_file);
        installed_data = json::parse(installed_packages);

        // checks if package is already installed
        if (installed_data["packages"].contains(requested_package))
        {
            fmt::print("\nThe package \"{}\" is already installed...\n\n", requested_package);
            break;
        };

        // checks if package exists in package.json
        if (package_data["packages"].contains(requested_package))
        {
            repo_name = requested_package;
            repository_URL = package_data["packages"][requested_package]["git_link"];

            // strips quotes from directory
            for (char c : testing)
            {
                if (c != '\"')
                {
                    result_string += c;
                }
            }

            // clone the repo
            command = fmt::format("cd {} && git clone {} target/debug/deps/{}", result_string, std::string(repository_URL), std::string(repo_name));
            fmt::print("Command: {}\n", command);
            return_code = system(command.c_str());

            // git version number
            command = fmt::format("cd {}/target/debug/deps/{} && git describe --tags --abbrev=0", result_string, std::string(repo_name));
            repo_version = exec(command.c_str());
            repo_version.erase(std::remove(repo_version.begin(), repo_version.end(), '\n'), repo_version.end()); // removes new line character from version

            // assigning unknown if no version number exists in git repo
            if (repo_version.empty())
            {
                repo_version = "unknown";
            }

            // updating installed package manager
            std::ofstream output("installed_packages.json");
            installed_data["packages"][repo_name] = {
                {"git_link", repository_URL},
                {"repo_name", repo_name},
                {"version", repo_version},
            };

            output << installed_data.dump(4);
        }
        else
        {
            fmt::print("\nPackage \"{}\" does not exist. Please enter the correct package name.\n\n", requested_package);
        }

        break;

    /**
     * LIST PACKAGES
     */
    case PossibleOptions::LIST_PACKAGES:

        if (beldum.file_exists("installed_packages.json"))
        {
            installed_data = json::parse(installed_packages);

            // prints header
            std::cout << "\n\n\n";
            create_row_help_menu("PACKAGE:", "VERSION:");
            std::cout << std::setw(80) << std::left << "--------------------------------------------------------------------------------" << std::endl;
            for (auto test : installed_data["packages"])
            {
                // prints all packages
                create_row_help_menu(std::string(test["repo_name"]), std::string(test["version"]));
            }
            // space at bottom
            std::cout << "\n\n"
                      << std::endl;
        }
        else
        {
            std::cerr << "\nThe installed_packages.json does not exist, please run --init" << std::endl;
        }
        break;

    case PossibleOptions::UNINSTALL:

        installed_data = json::parse(installed_packages);
        if (installed_data["packages"].contains(requested_package))
        {
            fmt::print("\n\nUninstalling...\n");

            // remove the repo
            command = "rm -rf target/debug/deps/" + std::string(requested_package);
            fmt::print("Command: {}\n", command);
            return_code = system(command.c_str());

            // update installed_packages.json
            installed_data["packages"].erase(requested_package);
            std::ofstream output("installed_packages.json");
            output << installed_data.dump(4);

            fmt::print("\nPackage {} successfully uninstalled.\n\n", requested_package);

            break;
        }
        else
        {
            std::cout << "\n\nPackage " << requested_package << " does not exist. Please check the installed packages and try again...\n\n"
                      << std::endl;
            break;
        }

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

    auto help_cmd = app.add_subcommand("help", "Show help information");
    help_cmd->callback([this]()
                       { check_passed_shell_arguments(PossibleOptions::HELP); });

    auto version_cmd = app.add_subcommand("version", "Show version information");
    version_cmd->callback([this]()
                          { check_passed_shell_arguments(PossibleOptions::VERSION); });

    // Define subcommands with file dependencies
    std::string package_name;

    auto install_cmd = app.add_subcommand("install", "Install a dependency");
    install_cmd->add_option("package_name", package_name, "Name of the package to install")->required();
    install_cmd->callback([this, &package_name]()
                          {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists("package.json")) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::INSTALL); });

    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstall a dependency");
    uninstall_cmd->add_option("package_name", package_name, "Name of the package to uninstall")->required();
    uninstall_cmd->callback([this, &package_name]()
                            {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists("package.json")) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        individual_package.name = package_name;
        check_passed_shell_arguments(PossibleOptions::UNINSTALL); });

    auto list_cmd = app.add_subcommand("list", "List installed packages");
    list_cmd->callback([this]()
                       {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists("package.json")) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        check_passed_shell_arguments(PossibleOptions::LIST_PACKAGES); });

    auto clean_cmd = app.add_subcommand("clean", "Clean build directory and dependencies");
    clean_cmd->callback([this]()
                        {
        if (!beldum.file_exists("installed_packages.json") || !beldum.file_exists("package.json")) {
            std::cerr << "Error: Missing required files (installed_packages.json or package.json). Please run 'beldum init' first.\n";
            return;
        }
        check_passed_shell_arguments(PossibleOptions::CLEAN); });

    CLI11_PARSE(app, argc, argv);

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
