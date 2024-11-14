
#include <string>

#ifndef BELDUM_INSTALL_H
#define BELDUM_INSTALL_H

int beldum_install(std::string &requested_package,
                   std::string &repo_name,
                   std::string &repo_version,
                   std::string &repository_URL,
                   std::string &repo_type,
                   std::string &command,
                   std::string &package_directory_path,
                   std::string &installed_packages_path,
                   const std::string &cmake_list_path,
                   std::string &cmakeStaticCommand,
                   std::string &cmakeHeaderOnlyCommand);

#endif