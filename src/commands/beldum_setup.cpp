#include "header_files/beldum_setup.hpp"
#include "header_files/global_utilities.hpp"

#include <cstdlib> // for std::system
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h> // for chmod

#ifndef JSON_DEPENDENCY_EXIST
#error("JSON_DEPENDENCY_EXIST not defined");
#endif

#include "nlohmann/json.hpp"

namespace beldum_setup {
/**
 * main.cpp that's created during setup
 */
const std::string main_cpp = R"(
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
)";

/**
 * build script created during setup
 */
const std::string build_script = R"(
#!/bin/bash

# Define variables
BUILD_DIR="target/debug/build"
BUILD_TYPE="Debug" # Change to "Release" if needed

# Function to print error messages
function error_exit {
    echo "[ERROR] $1"
    exit 1
}

# Step 1: Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory at $BUILD_DIR..."
    mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory."
fi

# Step 2: Run CMake to configure the project
echo "Configuring project with CMake in $BUILD_DIR..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" || error_exit "CMake configuration failed."

# Step 3: Build the project
echo "Building project in $BUILD_DIR..."
cmake --build "$BUILD_DIR" --target all || error_exit "Build failed."

echo "Build complete!"
)";

/**
 * CMakeLists.txt that's created during setup
 */
const std::string c_make_lists = R"(

# CMake Gloabl Config
cmake_minimum_required(VERSION 3.10)
project(MyProject) # Set the project name
set(CMAKE_CXX_STANDARD 20) # Specify the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EXECUTABLE_NAME cpp_program_executable)

# Link Dependencies
set(BELDUM_LIB_DIR "${CMAKE_SOURCE_DIR}/target/debug/deps") # Set the path to the C++ libraries
link_directories(${BELDUM_LIB_DIR}) # Add the path to the linker

# Create executeable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/target/debug/build") # Set the output directory for the executable
add_executable(${EXECUTABLE_NAME} src/main.cpp) # Add the executable

##################################
# TODO: Add libraries not managed by beldum
# To add custom libraries, examples below: 
##################################

# find_package(CURL REQUIRED)
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE CURL::libcurl)







##################################
# Beldum Config -- DO NOT TOUCH --
##################################

# BELDUM-HEADER-ONLY header only library

# BELDUM-STATIC-ONLY static library

# BELDUM-LINKER

)";

void create_src_and_main() {

    std::ofstream output;
    BeldumLogging logger;

    if (!file_exists("src/main.cpp")) {
        std::cout << "Creating src/main.cpp" << std::endl;
        std::filesystem::create_directory("src");

        output.open("src/main.cpp");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open build.sh file.");
        }

        output << main_cpp;
        output.close();
    }
}

void create_build_script() {

    std::ofstream output;
    BeldumLogging logger;

    if (!file_exists("build.sh")) {
        std::cout << "Creating ./build.sh" << std::endl;
        output.open("build.sh");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open build.sh file.");
        }

        output << build_script;
        output.close(); // Close the file after writing

        // Make the file executable
        if (std::system("chmod +x build.sh") != 0) {
            std::cerr << "Failed to make build.sh executable." << std::endl;
        }
    }
}

void create_cmake_lists() {

    std::ofstream output;
    BeldumLogging logger;

    if (!file_exists("CMakeLists.txt")) {
        std::cout << "Creating ./CMakeLists.txt" << std::endl;

        output.open("CMakeLists.txt");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open CMakeLists.txt file.");
        }
        output << c_make_lists;
        output.close();
    }
}

void create_package_json(nlohmann::ordered_json &package_data, std::string &project_name) {
    std::ofstream output;
    BeldumLogging logger;

    if (!file_exists(beldum_json_path)) {
        fmt::print("Creating {}", beldum_json_path);
        std::cout << std::endl;
        output.open(beldum_json_path);
        if (!output.is_open()) {
            logger.logError("Error: Failed to open " + beldum_json_path + " file.");
        }

        package_data = {
            {"name", project_name},
            {"version", "1.0.0"},
            {"scripts",
             {{"build", "cmake -S . -B target/debug/build && cmake --build target/debug/build"},
              {"clean", "rm -rf ./target/debug/build"},
              {"execute", "./target/debug/build/cpp_program_executable"}}},
            {"dependencies", {}}};

        output << package_data.dump(4);

        output.close();
        fmt::print("{} has been created successfully.", beldum_json_path);
        std::cout << std::endl;
    }
}
} // namespace beldum_setup

int beldum_create_project(std::string &project_name) {
    using json = nlohmann::ordered_json;
    json installed_data;
    json beldum_data;
    int return_code = 0;

    // Create the project directory
    if (std::filesystem::exists(project_name)) {
        std::cerr << "Error: Project directory \"" << project_name << "\" already exists.\n";
        return 1;
    }
    if (!std::filesystem::create_directory(project_name)) {
        std::cerr << "Error: Failed to create project directory \"" << project_name << "\".\n";
        return 1;
    }
    std::cout << "Created project directory: " << project_name << "\n";

    // Change the working directory to the project directory
    std::filesystem::current_path(project_name);
    std::cout << "Changed working directory to: " << std::filesystem::current_path() << "\n";

    // creates packages folder if it doesn't exist
    std::cout << "\n";
    if (file_exists(beldum_json_path) && file_exists(available_packages_path)) {
        fmt::print("~/.beldum/packages/ and {} already exist.\nTry "
                   "installing an example package with --install example_package\n\n",
                   beldum_json_path);
        return_code = 1;
        return return_code;
    } else {
        beldum_setup::create_package_json(beldum_data, project_name);
        beldum_setup::create_build_script();
        beldum_setup::create_src_and_main();
        beldum_setup::create_cmake_lists();
    }

    std::cout << "\n";
    return return_code;
}

int beldum_init(std::string &project_name) {
    using json = nlohmann::ordered_json;
    json installed_data;
    json beldum_data;
    int return_code = 0;

    if (file_exists(beldum_json_path) && file_exists(available_packages_path)) {
        fmt::print("~/.beldum/packages/ and {} already exist.\nTry "
                   "installing an example package with --install example_package\n\n",
                   beldum_json_path);
        return_code = 1;
        return return_code;
    } else {
        beldum_setup::create_package_json(beldum_data, project_name);
        beldum_setup::create_build_script();
        beldum_setup::create_src_and_main();
        beldum_setup::create_cmake_lists();

        return return_code;
    }
}
