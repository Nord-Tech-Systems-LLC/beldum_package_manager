# General config
cmake_minimum_required(VERSION 3.16)
# BELDUM VERSION
project(beldum VERSION 1.0.0 LANGUAGES CXX)
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION_NUM}")

# Define C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Base directories for dependencies and build
set(DepsRootDir "${CMAKE_SOURCE_DIR}/target/debug/deps")
set(BuildRootDir "${CMAKE_SOURCE_DIR}/target/debug/build")

# Build output subdirectories
set(BuildBinDir "${BuildRootDir}/bin")
set(BuildLibDir "${BuildRootDir}/deps")

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BuildBinDir}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BuildLibDir}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BuildLibDir}")

# Compiler flags
set(CXXFLAGS "-Wall -Wextra -Wpedantic -Wconversion -Wshadow")
set(DEBUG_FLAGS "-ggdb3 -fsanitize=undefined,address")
set(RELEASE_FLAGS "-O2 -Werror")

# Set debug and release flags based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${DEBUG_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${RELEASE_FLAGS}")
endif()

# Source and include directories
set(SrcDir src)
set(IncludeDir ${SrcDir})
file(GLOB_RECURSE SOURCES "${SrcDir}/*.cpp")
include_directories(${IncludeDir} ${DepsRootDir})

# Handle JSON dependency
execute_process(COMMAND git clone https://github.com/nlohmann/json.git ${DepsRootDir}/json)
add_subdirectory(${DepsRootDir}/json EXCLUDE_FROM_ALL)  # nlohmann_json
add_definitions(-DJSON_DEPENDENCY_EXIST)

# Handle FMT dependency
execute_process(COMMAND git clone https://github.com/fmtlib/fmt.git ${DepsRootDir}/fmt)
add_subdirectory(${DepsRootDir}/fmt EXCLUDE_FROM_ALL)  # fmt
add_definitions(-DFMT_DEPENDENCY_EXIST)

# Handle CLI11 dependency
execute_process(COMMAND git clone https://github.com/CLIUtils/CLI11.git ${DepsRootDir}/CLI11)
add_subdirectory(${DepsRootDir}/CLI11 EXCLUDE_FROM_ALL)  # CLI11
add_definitions(-DCLI11_DEPENDENCY_EXIST)

# Handle spdlog dependency
execute_process(COMMAND git clone https://github.com/gabime/spdlog ${DepsRootDir}/spdlog)
add_subdirectory(${DepsRootDir}/spdlog EXCLUDE_FROM_ALL)  # CLI11
add_definitions(-DSPDLOG_DEPENDENCY_EXIST)

# List libraries to link
set(MY_LIBRARIES
    nlohmann_json::nlohmann_json    # From json subdirectory
    fmt::fmt                        # From fmt library
    CLI11::CLI11                    # From CLI11 library
    spdlog::spdlog                  # From spdlog library
)

# Create executable
add_executable(beldum ${SOURCES})
foreach(CPP_LIB ${MY_LIBRARIES})
    target_link_libraries(beldum PRIVATE ${CPP_LIB})
endforeach()

# Set link flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(beldum PRIVATE -fsanitize=undefined,address)
endif()

# Install target
install(TARGETS beldum DESTINATION /usr/local/bin)

# Custom clean target to delete the build directory contents
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BuildRootDir}
    COMMENT "Cleaning up the entire build directory"
)
