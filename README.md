# BELDUM C++ PACKAGE MANAGER

![Open](https://img.shields.io/badge/Open-Source-89E051?style=flat) 
![License](https://img.shields.io/badge/License-Nord_Tech_Systems-C23656?style=flat) 
![C++](https://img.shields.io/badge/C++-Package_Manager-F34B7D?style=flat) 
![Maintainer](https://img.shields.io/badge/Maintainer-VikingOfValhalla-124ED8?style=flat)
  
The Beldum Package Manager is a lightweight, user-friendly package manager for C++ projects, designed to streamline the management and integration of dependencies. It allows developers to easily install, uninstall, and manage custom libraries needed for their C++ applications, much like Cargo does for Rust. Beldum emphasizes simplicity and ease of use, enabling rapid configuration with minimal overhead.

### **Supported Operating Systems**: Linux

## Key Features
- **Dependency Management**: Automatically resolves and installs dependencies, handling complex dependency trees with ease.
- **Local Caching and Logging**: Packages are cached locally, with detailed logs stored under `~/.beldum/logs/`, allowing for efficient builds and tracking.
- **Centralized Package List**: Maintains a list of available packages in `~/.beldum/packages/`, making it easy to browse and search for dependencies.
- **CMake Integration**: Designed to work seamlessly with CMake, Beldum can generate necessary build configurations to integrate libraries directly into the CMake build system.

**Side Note**: This library only supports a few static / header libraries at this time, but it will grow as libraries are tested. If you find a package you would like added, feel free to contact the maintainer to have the library added.

## Dependencies:

### Not included in beldum installation:
-   `git` git version control, need to configure git SSH

### Included in beldum installation:
-   `json` JSON C++ library
-   `fmt` fmt C++ library
-   `CLI11` CLI parser C++ library

## To Install

Execute `./build.sh` in the root project directory

## Commands

```bash
beldum [OPTIONS] [SUBCOMMAND]

  Beldum Package Manager

OPTIONS:
  -h,     --help              Print this help message and exit

SUBCOMMANDS:
  init                        Initialize new Beldum project
  create                      Create new Beldum project
  version                     Show version information
  install                     Install a dependency
  uninstall                   Uninstall a dependency
  list                        List installed packages
  run                         Run Beldum script
```

### Example Usage:

`beldum list --available` to list available packages  
`beldum list --installed` to list installed packages  
`beldum install package_name` to install packages  
`beldum uninstall package_name` to uninstall packages
`beldum run build` to build project based on beldum.json build script
`beldum run execute` to execute / run project based on beldum.json
`beldum run clean` to clean build directory based on beldum.json   
`beldum init` to initialize new project  
`beldum clean` to remove contents from cpp_libs folder
`beldum --help` to show commands 

### Main Project Workflow

1. Execute `beldum init` to initialize a project directory.
2. Execute `beldum install example_package` to install the preferred package.
3. Link the package in the `CMakeLists.txt` to import into the project using the template provided during `beldum init`.
4. Execute `beldum run build` to build project.
5. Execute `beldum run execute` to execute main program executable.
