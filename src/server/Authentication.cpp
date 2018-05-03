#include <accordserver/Authenticaion.h>
#include <accordserver/database/Database.h>
#include <accordserver/log/Logger.h>
#include <accordserver/util/CryptoUtil.h>

#include <argon2.h>

namespace accord {

std::map<mysqlpp::sql_bigint_unsigned, std::string> Authentication::tokens;
std::mutex Authentication::tokensMutex;

bool Authentication::registerUser(database::Database &database,
                                  std::string name, std::string email,
                                  std::string password)
{
    /*
     * 1. check if user exists
     * 2. generate ID
     * 3. gen salt
     * 4. gen hash
     * 5. store all in database
     */
    database::table_users user = database.getUser(email, password);
    if (user.table != NULL)
        return false;
    uint64_t id = util::CryptoUtil::getRandomUINT64();
    std::string salt = util::CryptoUtil::getRandomString(SALT_LEN);

    char buffer[32];
    argon2i_hash_raw(2, (1 << 16), 1, password.c_str(), password.length(),
                     salt.c_str(), salt.length(), buffer, 32);
    std::string hash(buffer);

    return database.initUser(id, name, email, hash, salt);
}


/*
 * Returns session token and puts it in the map
 * the function has errored when it returns an empty string
 */
std::string Authentication::authUser(database::Database &database,
                              std::string login, std::string password)
{

    database::table_users user = database.getUser(login, password);

    std::unique_lock<std::mutex> lock(tokensMutex);
    auto it = tokens.find(user.id());
    if (it != tokens.end())
        return tokens[user.id()];
    lock.unlock();

    std::string storedHash(user.password());
    std::string salt(user.salt());
    if (salt.length() != SALT_LEN) {
        log::Logger::log(log::ERROR, "Login " + login + " has invalid salt!");
        return std::string("");
    }
    char hash[32];
    argon2i_hash_raw(2, (1 << 16), 1, password.c_str(), password.length(),
                     salt.c_str(), salt.length(), hash, 32);
    std::string hashString(hash);
    if (hashString != storedHash)
        return std::string("invalid");

    std::string token = util::CryptoUtil::getRandomString(32);
    if (token.empty())
        return std::string("");

    lock.lock();
    tokens.insert(std::make_pair(user.id(), token));
    return token;
}

} /* namespace accord */
