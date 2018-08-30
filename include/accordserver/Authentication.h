#ifndef ACCORD_AUTHENTICATION_H
#define ACCORD_AUTHENTICATION_H

#include <string>
#include <mutex>
#include <stddef.h>
#include <accordserver/database/Database.h>
#include <boost/bimap.hpp>

namespace accord {

#define SALT_LEN 32
#define TOKEN_LEN 32

using TokensMapType = boost::bimap<uint64_t, std::string>;

class Authentication {
public:
    static bool registerUser(database::Database &database, std::string name,
                             std::string email, std::string password);
    static std::string authUser(database::Database &database,
                         std::string login, std::string password);
    static bool checkToken(const std::string &token);
private:
    static TokensMapType tokens;
    static std::mutex tokensMutex;
};

} /* namespace accord */
#endif
