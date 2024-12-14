
# Create new project

## Dependencies:
- `sudo apt install mysql-server`
- Verify mysql server is running: `sudo service mysql status`
- Login as root into mysql server: `sudo mysql`
- Create a new user `CREATE USER 'sammy'@'localhost' IDENTIFIED BY 'password';`
- Grant privileges to new created user: `GRANT CREATE, ALTER, DROP, INSERT, UPDATE, DELETE, SELECT, REFERENCES, RELOAD on *.* TO 'sammy'@'localhost' WITH GRANT OPTION;`
- Flush privileges: `FLUSH PRIVILEGES;`
- Exit mysql with `exit`
- Test login to new user with: `mysql -u sammy -p` and enter password you used to create
- Exit mysql and use those credentials you used to create user to test mysql library below:

```
beldum create mysql_test && cd mysql_test
beldum install mysql
```

### Paste the following in `main.cpp`:
```cpp
#include <iostream>
#include <mysql/mysql.h>
#include <vector>

class DatabaseConnection {
  private:
    MYSQL *conn = mysql_init(NULL);

  public:
    DatabaseConnection() {

        if (conn == NULL) {
            throw std::runtime_error("Error initializing MySQL connection");
        }

        if (mysql_real_connect(
                conn, "localhost", "username", "password", "database", 3306, NULL, 0) == NULL) {
            mysql_close(conn);
            throw std::runtime_error("Error connecting to MySQL database: " +
                                     std::string(mysql_error(conn)));
        }
    }

    ~DatabaseConnection() {
        if (conn) {
            mysql_close(conn);
        }
    }

    std::vector<std::vector<std::string>> executeQuery(const std::string &query) {
        std::vector<std::vector<std::string>> results;

        if (mysql_query(conn, query.c_str())) {
            throw std::runtime_error("Error executing query: " + std::string(mysql_error(conn)));
        }

        MYSQL_RES *mysql_res = mysql_store_result(conn);
        if (mysql_res == NULL) {
            throw std::runtime_error("Error storing result: " + std::string(mysql_error(conn)));
        }

        unsigned int num_fields = mysql_num_fields(mysql_res);
        MYSQL_ROW row;

        while ((row = mysql_fetch_row(mysql_res)) != NULL) {
            std::vector<std::string> current_row;
            for (unsigned int i = 0; i < num_fields; i++) {
                current_row.push_back(row[i] ? row[i] : "NULL");
            }
            results.push_back(current_row);
        }

        mysql_free_result(mysql_res);
        return results;
    }
};

int main() {
    DatabaseConnection db;
    auto results = db.executeQuery("SELECT * FROM Persons");
    // print values
    for (const auto &innerVector : results) {
        for (const auto &str : innerVector) {
            std::cout << str << " ";
        }
        std::cout << std::endl; // newline after each inner vector
    }
    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```