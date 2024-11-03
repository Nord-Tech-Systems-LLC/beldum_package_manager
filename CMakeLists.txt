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

# Set debug and release flags based on build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${DEBUG_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXFLAGS} ${RELEASE_FLAGS}")
endif()

# Add source files
file(GLOB_RECURSE SOURCES "${SrcDir}/*.cpp")

# Add include directories
include_directories(${IncludeDir} ${LibDir})

# Handle JSON dependency
execute_process(COMMAND git clone git@github.com:nlohmann/json.git ${LibDir}/json)
add_subdirectory(${LibDir}/json)  # nlohmann_json
add_definitions(-DJSON_DEPENDENCY_EXIST)

# Handle FMT dependency
execute_process(COMMAND git clone git@github.com:fmtlib/fmt.git ${LibDir}/fmt)
add_subdirectory(${LibDir}/fmt)  # fmt
add_definitions(-DFMT_DEPENDENCY_EXIST)

# Handle CLI11 dependency
execute_process(COMMAND git clone https://github.com/CLIUtils/CLI11 ${LibDir}/CLI11)
add_subdirectory(${LibDir}/CLI11)  # fmt
add_definitions(-DCLI11_DEPENDENCY_EXIST)

# List your libraries to link
set(MY_LIBRARIES
    nlohmann_json::nlohmann_json    # From json subdirectory
    fmt::fmt                        # From fmt library
    CLI11::CLI11                    # From CLI11 library
)

# Create the executable
add_executable(beldum ${SOURCES})

# Link each library in MY_LIBRARIES
message("\n\nLibraries Linked:")
foreach(CPP_LIB ${MY_LIBRARIES})
    message("Library Name: ${CPP_LIB}")
    target_link_libraries(beldum PRIVATE ${CPP_LIB})  # Link with the executable name
endforeach()
message("\n\n")

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
