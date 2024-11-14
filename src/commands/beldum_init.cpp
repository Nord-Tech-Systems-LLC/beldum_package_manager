#include "header_files/beldum_init.hpp"
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

int beldum_init(std::string &installed_packages_path, std::string &packages_path) {
    using json = nlohmann::json;
    BeldumInit beldum;
    json installed_data;
    int return_code = 0;

    // creates packages folder if it doesn't exist
    std::cout << "\n";
    if (file_exists(installed_packages_path) && file_exists(packages_path)) {
        fmt::print("~/.beldum/packages/ and installed_packages.json already exist.\nTry "
                   "installing an example package with --install example_package\n\n");
        return_code = 1;
        return return_code;
    } else {
        beldum.create_installed_packages(installed_data);
        // beldum.create_package_json(package_data);
        beldum.create_build_script();
        beldum.create_src_and_main();
        beldum.create_cmake_lists();
    }

    std::cout << "\n";
    return return_code;
}

void BeldumInit::create_src_and_main() {
    std::string mainCpp = R"(
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
)";

    if (!file_exists("src/main.cpp")) {
        std::cout << "Creating src/main.cpp" << std::endl;
        std::filesystem::create_directory("src");

        output.open("src/main.cpp");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open build.sh file.");
        }

        output << mainCpp;
        output.close();
    }
}

void BeldumInit::create_build_script() {
    std::string script = R"(
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

    if (!file_exists("build.sh")) {
        std::cout << "Creating ./build.sh" << std::endl;
        output.open("build.sh");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open build.sh file.");
        }

        output << script;
        output.close(); // Close the file after writing

        // Make the file executable
        if (std::system("chmod +x build.sh") != 0) {
            std::cerr << "Failed to make build.sh executable." << std::endl;
        }
    }
}

void BeldumInit::create_cmake_lists() {
    std::string cMakeLists = R"(

# CMake Gloabl Config
cmake_minimum_required(VERSION 3.10)
project(MyProject) # Set the project name
set(CMAKE_CXX_STANDARD 20) # Specify the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Link Dependencies
set(BELDUM_LIB_DIR "${CMAKE_SOURCE_DIR}/target/debug/deps") # Set the path to the C++ libraries
link_directories(${BELDUM_LIB_DIR}) # Add the path to the linker

# Create executeable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/target/debug/build") # Set the output directory for the executable
add_executable(MyExecutable src/main.cpp) # Add the executable

##################################
# TODO: Add libraries not managed by beldum
# To add custom libraries, examples below: 
##################################

# find_package(CURL REQUIRED)
# target_link_libraries(MyExecutable PRIVATE CURL::libcurl)







##################################
# Beldum Config -- DO NOT TOUCH --
##################################

# BELDUM-HEADER-ONLY header only library

# BELDUM-STATIC-ONLY static library

foreach(CPP_LIB ${MY_LIBRARIES})
    message("Library Name: ${BELDUM_LIB_DIR}")
    target_link_libraries(MyExecutable PRIVATE ${CPP_LIB})
endforeach()
message("\n\n")

)";
    if (!file_exists("CMakeLists.txt")) {
        std::cout << "Creating ./CMakeLists.txt" << std::endl;

        output.open("CMakeLists.txt");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open CMakeLists.txt file.");
        }
        output << cMakeLists;
        output.close();
    }
}

void BeldumInit::create_package_json(nlohmann::json &package_data) {
    if (!file_exists("package.json")) {
        std::cout << "Creating package.json" << std::endl;
        output.open("package.json");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open package.json file.");
        }

        package_data["packages"] = {
            {"example_package",
             {{"git_link", "git@github.com:Nord-Tech-Systems-LLC/example_package.git"}}}};

        output << package_data.dump(4);

        output.close();
    }
}

void BeldumInit::create_installed_packages(nlohmann::json &installed_data) {
    if (!file_exists("installed_packages.json")) {
        std::cout << "Creating installed_packages.json" << std::endl;

        output.open("installed_packages.json");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open installed_packages.json file.");
        }
        installed_data["packages"] = {};
        output << installed_data.dump(4);
        output.close();
    }
}