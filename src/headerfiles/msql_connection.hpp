// #include <mysqlx/xdevapi.h>
/**
 * Usage example for Driver, Connection, (simple) Statement, ResultSet
 */

class MySQLConnection {
   public:
    static MySQLConnection &getInstance() {
        static MySQLConnection instance;
        return instance;
    };
    // ~MySQLConnection();
    void login(std::string &host, std::string &username, std::string &password);

   private:
    std::string execute_command(const char *command);
};
