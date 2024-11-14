#pragma once

#ifndef BELDUM_INIT_H
#define BELDUM_INIT_H

#include "beldum_logging.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

class BeldumInit {
  public:
    void create_src_and_main();
    void create_build_script();
    void create_cmake_lists();
    void create_package_json(nlohmann::json &package_data);
    void create_installed_packages(nlohmann::json &installed_data);
    std::ofstream output;

  private:
    BeldumLogging logger;
};

int beldum_init(std::string &installed_packages_path, std::string &available_packages_path);

#endif