// #include <mysqlx/xdevapi.h>
// #include <cppconn/driver.h>
// #include <mysql_connection.h>
// #include <mysql_driver.h>

#include <mysql/mysql.h>
// #include <cppconn/connection.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "headerfiles/msql_connection.hpp"

void MySQLConnection::login(char *hostname, char *username, char *password) {
    try {
        /* Using the Driver to create a connection */
        char *database = "beldum_package_manager";
        MYSQL *connection;
        MYSQL_RES *result;
        MYSQL_ROW row;

        const char *statement = "select * from packages";
        if (!(connection = mysql_init(NULL))) {
            puts("Initialization has failed!");
        }
        if (connection = mysql_real_connect(connection, hostname, username, password, database, 3306, NULL, 0)) {
            std::cout << "Attempting to connect to database..."
                      << "\n";
            std::cout << "Host: " << hostname << "\n";
            std::cout << "User: " << username << std::endl;
            std::cout << "Connected to MySQL Database." << std::endl;
        }
        if (mysql_query(connection, statement)) {
            std::cout << "Attempting to conduct query..." << std::endl;
        }
        result = mysql_store_result(connection);
        int num_fields = mysql_num_fields(result);
        std::cout << "Data output below: " << std::endl;
        while ((row = mysql_fetch_row(result))) {
            for (int i = 0; i < num_fields; i++) {
                printf("%s ", row[i] ? row[i] : "NULL");
            }

            printf("\n");
        }

        mysql_free_result(result);
        mysql_close(connection);

        // sql::Driver *driver = nullptr;
        // sql::Connection *connection = nullptr;

        // driver = get_driver_instance();
        // connection = driver->connect(host, username, password);
        // driver = get_driver_instance();
        // const std::string user = (EXAMPLE_USER);
        // const std::string pass = (EXAMPLE_PASS);
        // const std::string database = (EXAMPLE_DB);

        // delete con;

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