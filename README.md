# BELDUM C++ PACKAGE MANAGER

The Beldum C++ Package Manager is built to simplify the C++ workflow and utilize header libraries.

**Side Note**:
This package manager only supports header libraries at this time.  
**Supported Operating Systems: Linux**

### Dependencies:

-   `json` C++ library
-   `git` git version control

### To Install

Execute `./build.sh` in the root project directory

### Commands

`beldum --init` to initialize new project  
`beldum --list` to list installed packages  
`beldum --install` to install packages  
`beldum --uninstall` to uninstall packages  
`beldum --help` to show commands  
`beldum --clean` to remove contents from cpp_libs folder

### Main Project Workflow

1. Execute `beldum --init` to initialize a project directory.
2. Update the package.json with a package from GitHub using this format below:

```json
{
    "packages": {
        "example_package": {
            "git_link": "git@github.com:Nord-Tech-Systems-LLC/example_package.git"
        },
        "new_package": {
            "git_link": "git@github.com:username/new_package.git"
        }
    }
}
```

3. Execute `beldum --install example_package` to install the preferred package.
