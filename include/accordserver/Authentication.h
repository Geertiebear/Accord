#ifndef ACCORD_AUTHENTICATION_H
#define ACCORD_AUTHENTICATION_H

#include <map>
#include <mutex>
#include <stddef.h>
#include <accordserver/database/Database.h>

namespace accord {

#define SALT_LEN 32

class Authentication {
public:
    static bool registerUser(database::Database &database, std::string name,
                             std::string email, std::string password);
    static std::string authUser(database::Database &database,
                         std::string login, std::string password);
private:
    static std::map<mysqlpp::sql_bigint_unsigned, std::string> tokens;
    static std::mutex tokensMutex;
};

} /* namespace accord */
#endif
