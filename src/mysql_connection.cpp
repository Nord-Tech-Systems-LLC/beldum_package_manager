// #include <mysqlx/xdevapi.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "headerfiles/msql_connection.hpp"

void MySQLConnection::login(std::string &host, std::string &username, std::string &password) {
    try {
        // const std::string user = (EXAMPLE_USER);
        // const std::string pass = (EXAMPLE_PASS);
        // const std::string database = (EXAMPLE_DB);
        std::cout << "Attempting to connect to database..."
                  << "\n";
        std::cout << "Host: " << host << "\n";
        std::cout << "User: " << username << std::endl;

    } catch (std::string &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        /* Use what() (derived from std::runtime_error) to fetch the error message */
        // std::cout << "# ERR: " << e.what();
        // std::cout << " (MySQL error code: " << e.getErrorCode();
        // std::cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
};

std::string MySQLConnection::execute_command(const char *command) {
    // pipes command result to string for use
    std::array<char, 1024> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}