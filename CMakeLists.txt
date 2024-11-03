cmake_minimum_required(VERSION 3.16)
project(beldum VERSION 1.0 LANGUAGES CXX)

# Define C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the directories
set(SrcDir src)
set(IncludeDir ${SrcDir})
set(LibDir target/debug/deps)

# Specify the new build directory structure
set(BuildDir target/debug/build)
set(BuildBinDir ${BuildDir}/bin)
set(BuildObjectsDir ${BuildDir}/objects)

# Output binary path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildBinDir})

# Compiler flags
set(CXXFLAGS "-Wall -Wextra -Wpedantic -Wconversion -Wshadow")
set(DEBUG_FLAGS "-ggdb3 -fsanitize=undefined,address")
set(RELEASE_FLAGS "-O2 -Werror")

# Add include directories
include_directories(${IncludeDir} ${LibDir})

# Detect and link JSON and FMT libraries if they exist
set(JSON_DEPENDENCY_PATH "${LibDir}/json/single_include/nlohmann/json.hpp")
set(FMT_DEPENDENCY_PATH "${LibDir}/fmt/include/fmt/core.h")

# Check if JSON dependency exists
if (EXISTS ${JSON_DEPENDENCY_PATH})
    message(STATUS "JSON library dependency found.")
    add_definitions(-DJSON_DEPENDENCY_EXIST)
else()
    message(STATUS "JSON library dependency not found. Downloading...")
    execute_process(COMMAND git clone git@github.com:nlohmann/json.git ${LibDir}/json)
endif()

# Set debug and release flags based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${DEBUG_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${RELEASE_FLAGS}")
endif()

# Add source files
file(GLOB_RECURSE SOURCES "${SrcDir}/*.cpp")

# Create the executable
add_executable(beldum ${SOURCES})

# Set link flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(beldum PRIVATE -fsanitize=undefined,address)
endif()

# Installation
install(TARGETS beldum DESTINATION /usr/local/bin)

# Custom clean target to delete the target/debug/build directory
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BuildDir}
    COMMENT "Cleaning up the entire target/debug/build directory"
)
