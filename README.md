# CPP_DEPENDENCY MANAGER

The cpp dependency manager only works with header included packages. The purpose of the package is to be

### Dependencies:

-   `json` C++ library
-   `git` git version control

### To Install

Execute `make` in the root project directory

### Commands

`beldum --init` to initialize new project  
`beldum --list` to list installed packages  
`beldum --install` to install packages  
`beldum --uninstall` to uninstall packages  
`beldum --help` to show commands  
`beldum --clean` to remove contents from cpp_libs folder

### Main Project Workflow

1. Execute `beldum --init` to initialize a project directory.
2. Update the packages.json with a package from GitHub using this format below:

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
