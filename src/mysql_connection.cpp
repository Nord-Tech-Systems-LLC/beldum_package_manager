#include <mysql/mysql.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "headerfiles/msql_connection.hpp"

MySQLConnection::~MySQLConnection() {
    mysql_close(connection);
}

void MySQLConnection::connectToDatabase(const char *hostname,
                                        const char *database,
                                        const char *username,
                                        const char *password) {
    try {
        // if failed initializing
        if (!(connection = mysql_init(NULL))) {
            std::cout << "Initialization has failed!" << std::endl;
        }
        // if failed connection to database
        if (!(connection = mysql_real_connect(connection, hostname, username, password, database, 3306, NULL, 0))) {
            throw std::invalid_argument("Connection has failed!");
        } else {
            // connect to database
            std::cout << "Attempting to connect to database..."
                      << "\n";
            std::cout << "Host: " << hostname << "\n";
            std::cout << "User: " << username << std::endl;
            std::cout << "Connected to MySQL Database: " << database << std::endl;
        }

    } catch (std::exception &e) {
        // error location
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << "\n";
        // error message
        std::cout << "# ERR: " << e.what() << std::endl;
    }
};

void MySQLConnection::executeQuery(const char *statement) {
    MYSQL_RES *result;
    MYSQL_ROW row;

    if (mysql_query(connection, statement)) {
        std::cout << "Attempting to conduct query..." << std::endl;
    }

    result = mysql_store_result(connection);
    int num_fields = mysql_num_fields(result);

    // print data
    std::cout << "Data output below: " << std::endl;
    while ((row = mysql_fetch_row(result))) {
        for (int i = 0; i < num_fields; i++) {
            printf("%s ", row[i] ? row[i] : "NULL");
        }

        printf("\n");
    }

    mysql_free_result(result);
}