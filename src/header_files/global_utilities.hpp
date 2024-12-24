

#ifndef GLOBAL_UTIL_H
#define GLOBAL_UTIL_H

#include "header_files/beldum_logging.hpp"
#include "nlohmann/json.hpp"
#include <string>

extern std::string beldum_json_path;
extern std::string available_packages_path;
extern const std::string cmake_list_path;
std::string execute_command_return_result(std::string cmd);
bool show_warning(std::string warning_message);
bool file_exists(const std::string &name);
bool ends_with(std::string_view str, std::string_view suffix);
bool starts_with(std::string_view str, std::string_view prefix);

int open_file_parse_json(const std::string &file_path, nlohmann::ordered_json &out_json);
int write_json_to_file(const std::string &file_path, const nlohmann::ordered_json &data_to_write);
int read_file_to_vector(const std::string &file_path, std::vector<std::string> &out_vector);
int write_vector_to_file(const std::string &file_path, std::vector<std::string> &out_vector);

int execute_command_with_spinner(const std::string &command);
void execute_system_command(const std::string &command);
std::string build_static_library_cmake_input(std::string package_name);
std::string build_header_only_library_cmake_input(std::string package_name);

#endif