

#ifndef GLOBAL_UTIL_H
#define GLOBAL_UTIL_H

#include "header_files/beldum_logging.hpp"
#include <string>

extern std::string beldum_json_path;
extern std::string available_packages_path;
std::string execute_command_return_result(std::string cmd);
bool show_warning();
bool file_exists(const std::string &name);
bool ends_with(std::string_view str, std::string_view suffix);
bool starts_with(std::string_view str, std::string_view prefix);
int execute_command_with_spinner(const std::string &command);
void execute_system_command(const std::string &command);

#endif