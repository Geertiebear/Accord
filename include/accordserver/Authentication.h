#ifndef ACCORD_AUTHENTICATION_H
#define ACCORD_AUTHENTICATION_H

#include <ctime>
#include <string>
#include <mutex>
#include <stddef.h>
#include <accordserver/database/Database.h>
#include <accordshared/types/Return.h>
#include <vector>

namespace accord {

#define SALT_LEN 32
#define TOKEN_LEN 64
#define TOKEN_LIFETIME 3600

class Authentication {
public:
    static bool registerUser(database::Database &database, std::string name,
                             std::string email, std::string password);
    static types::Token authUser(database::Database &database,
                         std::string login, std::string password);
    static bool authUser(const types::Token &token);
    static bool checkToken(const std::string &token);
private:
    static std::vector<char> constructVectorFromTokenData(
            const std::string &key, uint64_t id, std::time_t expiration);

    static std::vector<std::string> tokens;
    static std::mutex tokensMutex;
};

} /* namespace accord */
#endif
