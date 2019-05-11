#include <accordserver/Authentication.h>
#include <accordserver/database/Database.h>
#include <accordserver/log/Logger.h>
#include <accordserver/util/CryptoUtil.h>

#include <algorithm>
#include <argon2.h>
#include <chrono>

namespace accord {

std::vector<std::string> Authentication::tokens;
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
    auto user = database.getUser(email);
    if (user)
        return false;
    uint64_t id = util::CryptoUtil::getRandomUINT64();
    log::Logger::log(log::INFO, std::to_string(id));
    std::string salt = util::CryptoUtil::getRandomString(SALT_LEN);

    std::vector<char> buffer;
    buffer.resize(32);
    argon2i_hash_raw(2, (1 << 16), 1, password.c_str(), password.length(),
                     salt.c_str(), salt.length(), &buffer[0], 32);
    std::string hash = util::CryptoUtil::charToHex(buffer);

    return database.initUser(id, name, email, hash, salt);
}


/*
 * Returns session token and puts it in the map
 * the function has errored when it returns an empty string
 */
types::Token Authentication::authUser(database::Database &database,
                              std::string login, std::string password)
{
    log::Logger::log(log::DEBUG, "Authenticating user!");
    auto user = database.getUser(login);
    if (!user)
        return types::Token();

    std::string storedHash(user.get().password);
    std::string salt(user.get().salt);
    if (salt.length() != SALT_LEN) {
        log::Logger::log(log::FATAL, "Login " + login + " has invalid salt!");
        return types::Token();
    }
    std::vector<char> buffer;
    buffer.resize(32);
    argon2i_hash_raw(2, (1 << 16), 1, password.c_str(), password.length(),
                     salt.c_str(), salt.length(), &buffer[0], 32);
    std::string hash = util::CryptoUtil::charToHex(buffer);
    if (hash != storedHash)
        return types::Token();

    std::string key = util::CryptoUtil::getRandomString(TOKEN_LEN);
    if (key.empty())
        return types::Token();

    std::time_t currentTime = std::chrono::system_clock::now().
            time_since_epoch().count();
    std::time_t expiration = currentTime + TOKEN_LIFETIME;

    std::vector<char> token = constructVectorFromTokenData(
                key, user.get().id, expiration);

    std::string tokenHash = util::CryptoUtil::sha256(token);
    std::unique_lock<std::mutex> lock(tokensMutex);
    tokens.push_back(tokenHash);

    types::Token res(tokenHash, key, user.get().id, expiration);
    log::Logger::log(log::DEBUG, "Successfly authenticated user!");
    return res;
}

bool Authentication::authUser(const types::Token &token)
{
    std::vector<char> toHash = constructVectorFromTokenData(
                token.key, token.id, token.expiration);
    auto hash = util::CryptoUtil::sha256(toHash);
    return hash == token.token && checkToken(hash);
}

bool Authentication::checkToken(const std::string &token)
{
    std::unique_lock<std::mutex> lock(tokensMutex);
    return std::find(tokens.begin(), tokens.end(), token) != tokens.end();
}

std::vector<char> Authentication::constructVectorFromTokenData(
        const std::string &key, uint64_t id, std::time_t expiration)
{
    std::vector<char> res;
    std::copy(key.begin(), key.end(), std::back_inserter(res));

    std::string userIdString = std::to_string(id);
    std::string timeString = std::to_string(expiration);
    std::copy(userIdString.begin(), userIdString.end(),
              std::back_inserter(res));
    std::copy(timeString.begin(), timeString.end(), std::back_inserter(res));
    return res;
}

} /* namespace accord */
