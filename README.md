# BELDUM C++ PACKAGE MANAGER

The Beldum Package Manager is a lightweight, user-friendly package manager for C++ projects, designed to streamline the management and integration of dependencies. It allows developers to easily install, uninstall, and manage custom libraries needed for their C++ applications, much like Cargo does for Rust. Beldum emphasizes simplicity and ease of use, enabling rapid configuration with minimal overhead.

## Key Features
- **Dependency Management**: Automatically resolves and installs dependencies, handling complex dependency trees with ease.
- **Local Caching and Logging**: Packages are cached locally, with detailed logs stored under `~/.beldum/logs/`, allowing for efficient builds and tracking.
- **Centralized Package List**: Maintains a list of available packages in `~/.beldum/packages/`, making it easy to browse and search for dependencies.
- **CMake Integration**: Designed to work seamlessly with CMake, Beldum can generate necessary build configurations to integrate libraries directly into the CMake build system.

**Side Note**: This library only supports a few libraries at this time, but it will grow as libraries are tested.
**Supported Operating Systems: Linux**

### Dependencies:

### Not included in beldum installation:
-   `git` git version control

### Included in beldum installation:
-   `json` JSON C++ library
-   `fmt` fmt C++ library
-   `CLI11` CLI parser C++ library

### To Install

Execute `./build.sh` in the root project directory

### Commands

`beldum --help` to show commands  
`beldum init` to initialize new project  
`beldum list --available` to list available packages  
`beldum list --installed` to list installed packages  
`beldum install package_name` to install packages  
`beldum uninstall package_name` to uninstall packages  
`beldum clean` to remove contents from cpp_libs folder

### Main Project Workflow

1. Execute `beldum init` to initialize a project directory.
2. Execute `beldum install example_package` to install the preferred package.
3. Link the package in the `CMakeLists.txt` to import into the project using the template provided during `beldum init`
