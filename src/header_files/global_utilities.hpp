

#ifndef EXEC_UTIL_H
#define EXEC_UTIL_H

#include <string>

std::string execute_command(std::string cmd);
bool show_warning();
bool file_exists(const std::string &name);

#endif