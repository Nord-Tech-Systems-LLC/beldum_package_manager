
#ifndef BELDUM_INIT_H
#define BELDUM_INIT_H

#include "header_files/beldum_logging.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

void create_src_and_main();
void create_build_script();
void create_cmake_lists();
void create_package_json(nlohmann::json &package_data, std::string &project_name);
void create_installed_packages(nlohmann::json &installed_data);

int beldum_create_project(std::string &installed_packages_path,
                          std::string &packages_path,
                          std::string &project_name);

int beldum_init(std::string &installed_packages_path,
                std::string &packages_path,
                std::string &project_name);

#endif