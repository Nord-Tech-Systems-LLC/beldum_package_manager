
#ifndef BELDUM_INIT_H
#define BELDUM_INIT_H

#include "header_files/beldum_logging.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

int beldum_create_project(std::string &installed_packages_path,
                          std::string &packages_path,
                          std::string &project_name);

int beldum_init(std::string &installed_packages_path,
                std::string &packages_path,
                std::string &project_name);

#endif