

#include "header_files/global_utilities.hpp"
#include "fmt/core.h"
#include "nlohmann/json.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib> // For getenv
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

/**
 * #####################################################################
 * GLOBAL VARIABLES THROUGHOUT PACKAGE MANAGER
 * #####################################################################
 */
std::string beldum_json_path = "beldum.json";
std::string available_packages_path = std::string(getenv("HOME")) + "/.beldum/packages/";
const std::string cmake_list_path = "CMakeLists.txt";

/**
 * Builds cmake syntax for STATIC libraries inputting / removing into CMakeLists.txt
 * @param package_name = name of specific package
 */
std::string build_static_library_cmake_input(std::string package_name) {
    return fmt::format("add_subdirectory(${{BELDUM_LIB_DIR}}/{})", package_name);
};

/**
 * Builds cmake syntax for HEADER ONLY libraries inputting / removing into CMakeLists.txt
 * @param package_name = name of specific package
 */
std::string build_header_only_library_cmake_input(std::string package_name) {
    return fmt::format("include_directories(${{BELDUM_LIB_DIR}}/{})", package_name);
};

/**
 * Executes command and returns result
 * @param command = command to be executed
 */
std::string execute_command_return_result(std::string cmd) {
    // define the specific function pointer type for the deleter
    using PipeDeleter = int (*)(FILE *);

    // pipe command result to string for use
    std::array<char, 1024> buffer;
    std::string result;

    // initialize pipe with popen and custom deleter
    std::unique_ptr<FILE, PipeDeleter> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    // read the output from the command
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

/**
 * Shows warning and cancels if N is selected
 * @param warning_message = message to be printed for the warning
 */
bool show_warning(std::string warning_message) {
    // display the warning message
    fmt::print("{}\n", warning_message);
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

/**
 * #####################################################################
 * READ / WRITE TO TEXT AND JSON FILES
 * #####################################################################
 */

/**
 * Open file and parse to json object for manipulating
 * @param file_path = path of the json file to be manipulated
 * @param out_json = json object to output to for manipulating
 */
int open_file_parse_json(const std::string &file_path, nlohmann::ordered_json &out_json) {
    BeldumLogging logger;
    std::ifstream file_stream;
    int return_code;

    file_stream.open(file_path);
    if (!file_stream.is_open()) {
        logger.logError("Error: Failed to open " + file_path + " file.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened " + file_path + " file.");

    try {
        out_json = nlohmann::ordered_json::parse(file_stream);
        logger.log("Parsed JSON data from " + file_path);
    } catch (const nlohmann::json::parse_error &e) {
        logger.logError("Failed to parse JSON data: " + std::string(e.what()));
        return_code = 1;
        return return_code;
    }

    file_stream.close();
    return return_code;
}

/**
 * Write JSON to file using contents of JSON object
 * @param file_path = path of the file to output
 * @param out_vector = JSON data to insert into file
 */
int write_json_to_file(const std::string &file_path, const nlohmann::ordered_json &data_to_write) {
    BeldumLogging logger;
    std::ofstream output;
    int return_code;

    output.open(file_path);
    if (!output.is_open()) {
        logger.logError("Error: Failed to open " + file_path + " file.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened and wrote to " + file_path + " file.");

    output << data_to_write.dump(4);
    output.close();
    return return_code;
}

/**
 * Open file and push to vector for manipulating
 * @param file_path = path of the file to be manipulated
 * @param out_vector = vector to output to for manipulating
 */
int read_file_to_vector(const std::string &file_path, std::vector<std::string> &out_vector) {
    BeldumLogging logger;
    std::ifstream file_stream;
    std::string end_of_line; // used for single cmake line
    int return_code;

    file_stream.open(file_path);
    if (!file_stream.is_open()) {
        logger.logError("Error: Failed to open " + file_path + " file for reading.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened " + file_path + " file for reading.");

    while (std::getline(file_stream, end_of_line)) {
        out_vector.push_back(end_of_line); // Add the current line to the vector
    }
    file_stream.close();
    return return_code;
}

/**
 * Write file using contents of vector
 * @param file_path = path of the file to output
 * @param out_vector = vector to insert into file
 */
int write_vector_to_file(const std::string &file_path, std::vector<std::string> &out_vector) {
    BeldumLogging logger;
    std::ofstream output;
    int return_code;

    output.open(file_path);
    if (!output.is_open()) {
        logger.logError("Error: Failed to open " + file_path + " file for writing.");
        return_code = 1;
        return return_code;
    }
    logger.log("Opened " + file_path + " file for writing.");
    for (const auto &output_line : out_vector) {
        output << output_line << '\n';
    }
    output.close();

    return return_code;
}

/**
 * #####################################################################
 * PROGRESS BAR
 * #####################################################################
 */

namespace progress_bar {
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

// Spinner function to display terminal animation
void spinner(std::atomic<bool> &is_running) {
    const char spinner_chars[] = {'|', '/', '-', '\\'};
    size_t spinner_index = 0;

    while (is_running) {
        printf("\r%c", spinner_chars[spinner_index]);
        fflush(stdout);
        spinner_index = (spinner_index + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Simulate a command execution with progress updates
int simulate_command_with_progress(std::atomic<bool> &is_running, const std::string &command) {
    for (int i = 1; i <= 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate work
        printProgress(i / 100.0);                                  // Update progress bar
    }
    is_running = false;             // Stop spinner after progress is complete
    return system(command.c_str()); // Execute the system command
}
} // namespace progress_bar
// Function to execute the system command
int execute_command_with_spinner(const std::string &command) {
    std::atomic<bool> is_running(true);

    // Start spinner in a separate thread
    std::thread spinner_thread(progress_bar::spinner, std::ref(is_running));

    // Simulate command execution with progress updates
    int return_code = progress_bar::simulate_command_with_progress(is_running, command);

    // Wait for the spinner to stop
    spinner_thread.join();

    // Finalize output
    std::cout << "\nCommand completed with return code: " << return_code << std::endl;
    return return_code;
}

void execute_system_command(const std::string &command) {
    BeldumLogging logger;
    int result = std::system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Command failed: " + command);
    }
    logger.log("Executed command: " + command);
}