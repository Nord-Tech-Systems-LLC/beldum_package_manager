
#include <string>

#ifndef BELDUM_UNINSTALL_H
#define BELDUM_UNINSTALL_H

int beldum_uninstall(std::string &requested_package,
                     std::string &repo_name,
                     std::string &repo_type,
                     std::string &command,
                     std::string &single_package_directory_path,
                     const std::string &cmake_list_path,
                     std::string &cmakeStaticCommand,
                     std::string &cmakeHeaderOnlyCommand);

#endif