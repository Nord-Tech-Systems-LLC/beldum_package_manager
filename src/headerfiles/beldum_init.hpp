#pragma once

#ifndef BELDUM_INIT_H
#define BELDUM_INIT_H

#include <iostream>
#include <fstream>
#include "beldum_logging.hpp"
#include "nlohmann/json.hpp"

class BeldumInit
{
public:
    bool file_exists(const std::string &name);
    void create_src_and_main();
    void create_build_script();
    void create_cmake_lists();
    void create_package_json(nlohmann::json &package_data);
    void create_installed_packages(nlohmann::json &installed_data);
    std::ofstream output;
private:
    BeldumLogging logger;
};
#endif