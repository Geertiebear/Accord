#ifndef ACCORD_DATABASE_DATABASE_H
#define ACCORD_DATABASE_DATABASE_H

#include <string>'
#include <mysql++.h>

namespace accord {
namespace database {

class Database {
public:
    Database(std::string name, std::string address, std::string user,
             std::string password, int port);
    ~Database();

    int connect();
private:
    std::string name, address, user, password;
    int port;
    mysqlpp::Connection connection;
};

} /* namespace database */
} /* namespace accord */
#endif
