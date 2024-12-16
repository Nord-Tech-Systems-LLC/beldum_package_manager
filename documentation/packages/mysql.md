
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
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>
#include <mysql_connection.h>

class DatabaseConnection {
  private:
    sql::Driver *driver;
    sql::Connection *connection;

  public:
    DatabaseConnection(const std::string &host,
                       const std::string &user,
                       const std::string &password,
                       const std::string &schema) {
        try {
            driver = get_driver_instance();
            connection = driver->connect(host, user, password);
            connection->setSchema(schema);
        } catch (sql::SQLException &e) {
            std::cerr << "Connection error: " << e.what() << std::endl;
            throw;
        }
    }

    ~DatabaseConnection() {
        if (connection) {
            delete connection;
        }
    }

    sql::ResultSet *executeQuery(const std::string &query) {
        try {
            sql::Statement *stmt = connection->createStatement();
            sql::ResultSet *res = stmt->executeQuery(query);
            delete stmt;
            return res;
        } catch (sql::SQLException &e) {
            std::cerr << "Query error: " << e.what() << std::endl;
            throw;
        }
    }

};


int main() {
    DatabaseConnection db("tcp://127.0.0.1:3306", "username", "password", "database");
    sql::ResultSet *results = db.executeQuery("SELECT * FROM Persons");
    // print values
    while (res->next()) {
        cout << "\t... MySQL replies: ";
        /* Access column data by alias or column name */
        cout << res->getString("_message") << endl;
        cout << "\t... MySQL says it again: ";
        /* Access column data by numeric offset, 1 is the first column */
        cout << res->getString(1) << endl;
    }
    return 0;
}
```

# Build project:
```
beldum run build
beldum run execute
```