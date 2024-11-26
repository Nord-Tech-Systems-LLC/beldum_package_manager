
#ifndef BELDUM_SETUP_H
#define BELDUM_SETUP_H

#include "header_files/beldum_logging.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

int beldum_create_project(std::string &project_name);

int beldum_init(std::string &project_name);

#endif