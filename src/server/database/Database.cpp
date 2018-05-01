#include <accordserver/database/Database.h>
#include <accordserver/log/Logger.h>

namespace accord {
namespace database {

/*
 * Database is organized in the following way. There are two basic tables that
 * need to be present all the time, users and communities. These hosue the
 * basics for all users and communities such as metadata and statistics. Then
 * for each user and community we create various tables to describe their
 * friends and channels. Each user/community specific table will be prefixed
 * by with community_[id]_[table] or user_[id]_[table].
 */

Database::Database(const DatabaseOptions &options)
    : options(options)
{

}

Database::~Database()
{
    if (connection.connected())
        connection.disconnect();
}

int Database::connect()
{
    connection.set_option(new mysqlpp::ReconnectOption(true));
    return connection.connect(options.name.c_str(), options.address.c_str(),
                              options.user.c_str(),
                       options.password.c_str(), options.port);
}

void Database::disconnect()
{
    connection.disconnect();
}

bool Database::verify()
{
    // right now just check if we have a users and communities table
    // there might be more to follow who knows
    mysqlpp::Query query = connection.query("SHOW TABLES LIKE 'users'");
    mysqlpp::StoreQueryResult res;
    if (res = query.store())
        if (res.empty())
            return false;
    query = connection.query("SHOW TABLES LIKE 'communities'");
    if (res = query.store())
        if (res.empty())
            return false;
    query = connection.query("SHOW TABLES LIKE 'friends'");
    if (res = query.store())
        if (res.empty())
            return false;
    return true;
}

bool Database::initDatabase()
{
    try {
        mysqlpp::Query query = connection.
                query("CREATE TABLE users (id BIGINT UNSIGNED, "
                      "name VARCHAR(255), profilepic "
                      "VARBINARY(65535), friends INT,"
                      "communities INT, email VARCHAR(255))");
        if (!query.execute())
            return false;
        query = connection.query(
                "CREATE TABLE communities (id BIGINT UNSIGNED,"
                "name VARCHAR(255), profilepic VARBINARY(65535),"
                "members INT, channels INT)");
        if (!query.execute())
            return false;
        query = connection.query(
                "CREATE TABLE friends (id BIGINT UNSIGNED, "
                "user1 BIGINT UNSIGNED, user2 BIGINT UNSIGNED,"
                "status ENUM('pending', 'accepted'))");
        if (!query.execute())
            return false;
    } catch (mysqlpp::BadQuery &e) {
        log::Logger::log(log::WARNING, "Bad query made in database init: " +
                         std::string(e.what()));
    }
    return true;
}

} /* namespace database */
} /* namespace accord */
