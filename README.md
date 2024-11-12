# BELDUM C++ PACKAGE MANAGER

The Beldum C++ Package Manager is built to simplify the C++ workflow and utilize libraries that work with CMake. 

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
