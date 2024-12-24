
#include <string>

#ifndef BELDUM_INSTALL_H
#define BELDUM_INSTALL_H

int beldum_install_single(std::string &requested_package,
                          std::string &repo_version,
                          std::string &package_directory_path);

int beldum_install_all();

#endif