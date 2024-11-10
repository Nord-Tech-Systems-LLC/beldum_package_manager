#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h> // for chmod
#include <cstdlib>    // for std::system
#include "headerfiles/beldum_init.hpp"

#ifndef JSON_DEPENDENCY_EXIST
#error("JSON_DEPENDENCY_EXIST not defined");
#endif

#include "nlohmann/json.hpp"

bool BeldumInit::file_exists(const std::string &name)
{ 
    return std::filesystem::exists(name.c_str());
}

void BeldumInit::create_src_and_main()
{
    std::string mainCpp = R"(
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
)";

    if (!file_exists("src/main.cpp"))
    {
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

void BeldumInit::create_build_script()
{
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

    if (!file_exists("build.sh"))
    {
        std::cout << "Creating ./build.sh" << std::endl;
        output.open("build.sh");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open build.sh file.");
        }

        output << script;
        output.close(); // Close the file after writing

        // Make the file executable
        if (std::system("chmod +x build.sh") != 0)
        {
            std::cerr << "Failed to make build.sh executable." << std::endl;
        }
    }
}

void BeldumInit::create_cmake_lists()
{
    std::string cMakeLists = R"(

# CMake Gloabl Config
cmake_minimum_required(VERSION 3.10)
project(MyProject) # Set the project name
set(CMAKE_CXX_STANDARD 17) # Specify the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Link Dependencies
set(LIB_DIR "${CMAKE_SOURCE_DIR}/target/debug/deps") # Set the path to the C++ libraries
link_directories(${LIB_DIR}) # Add the path to the linker

# Create executeable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/target/debug/build") # Set the output directory for the executable
add_executable(MyExecutable src/main.cpp) # Add the executable

##################################
# TODO: Add libraries that you're using
##################################

# Add each library as a subdirectory
# add_subdirectory(${LIB_DIR}/json)          # nlohmann_json
# add_subdirectory(${LIB_DIR}/fmt)           # fmt library

# List your libraries to link
# set(MY_LIBRARIES
#     nlohmann_json::nlohmann_json        # From json subdirectory
#     fmt::fmt                            # From fmt library
# )

# Link each library in MY_LIBRARIES
# message("\n\nLibraries Linked:")
# foreach(CPP_LIB ${MY_LIBRARIES})
#     message("Library Name: ${CPP_LIB}")
#     target_link_libraries(MyExecutable PRIVATE ${CPP_LIB})
# endforeach()
# message("\n\n")

# Include directories for headers if necessary
# include_directories(${CMAKE_SOURCE_DIR}/path/to/headers)

)";
    if (!file_exists("CMakeLists.txt"))
    {
        std::cout << "Creating ./CMakeLists.txt" << std::endl;

        output.open("CMakeLists.txt");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open CMakeLists.txt file.");
        }
        output << cMakeLists;
        output.close();
    }
}

void BeldumInit::create_package_json(nlohmann::json &package_data)
{
    if (!file_exists("package.json"))
    {
        std::cout << "Creating package.json" << std::endl;
        output.open("package.json");
        if (!output.is_open()) {
            logger.logError("Error: Failed to open package.json file.");
        }

        package_data["packages"] = {
            {"example_package", {{"git_link", "git@github.com:Nord-Tech-Systems-LLC/example_package.git"}}}};

        output << package_data.dump(4);
        
        output.close();
    }
}

void BeldumInit::create_installed_packages(nlohmann::json &installed_data)
{
    if (!file_exists("installed_packages.json"))
    {
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