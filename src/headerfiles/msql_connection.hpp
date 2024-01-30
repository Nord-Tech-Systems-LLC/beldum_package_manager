
/**
 * Usage example for Driver, Connection, (simple) Statement, ResultSet
 */

struct connection_details {
    const char *server, *user, *password, *database;
};

class MySQLConnection {
   public:
    static MySQLConnection &getInstance() {
        static MySQLConnection instance;
        return instance;
    };
    // ~MySQLConnection();
    void login(char *hostname, char *username, char *password);

   private:
    std::string execute_command(const char *command);
};
