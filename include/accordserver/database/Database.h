#ifndef ACCORD_DATABASE_DATABASE_H
#define ACCORD_DATABASE_DATABASE_H

#include <string>'
#include <mysql++.h>

namespace accord {
namespace database {

struct DatabaseOptions {
    DatabaseOptions() {}
    DatabaseOptions(std::string name, std::string address, std::string user,
                    std::string password, int port)
        : name(name), address(address), user(user), password(password),
    port(port) {}
    std::string name, address, user, password;
    int port;
};

class Database {
public:
    Database(const DatabaseOptions &options);
    ~Database();

    int connect();
    void disconnect();
    bool verify();
    bool initDatabase();
private:
    const DatabaseOptions &options;
    mysqlpp::Connection connection;
};

} /* namespace database */
} /* namespace accord */
#endif
