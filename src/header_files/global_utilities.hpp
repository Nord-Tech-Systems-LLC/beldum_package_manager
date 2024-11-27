

#ifndef GLOBAL_UTIL_H
#define GLOBAL_UTIL_H

#include <string>

extern std::string beldum_json_path;
extern std::string available_packages_path;
std::string execute_command(std::string cmd);
bool show_warning();
bool file_exists(const std::string &name);
bool ends_with(std::string_view str, std::string_view suffix);
bool starts_with(std::string_view str, std::string_view prefix);
int execute_command_with_spinner(const std::string &command);

#endif