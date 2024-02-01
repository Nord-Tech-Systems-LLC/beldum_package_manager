
#include <mysql/mysql.h>

#include <string>

class MySQLConnection {
   public:
    static MySQLConnection &getInstance() {
        static MySQLConnection instance;
        return instance;
    };
    ~MySQLConnection();
    void connectToDatabase(const char *hostname,
                           const char *database,
                           const char *username,
                           const char *password);
    void executeQuery(const char *statement);

   private:
    MYSQL *connection;
};
