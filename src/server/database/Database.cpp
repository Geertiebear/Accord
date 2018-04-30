#include <accordserver/database/Database.h>

namespace accord {
namespace database {

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
    return true;
}

bool Database::initDatabase()
{
    mysqlpp::Query query = connection.query("CREATE TABLE users (temp INT)");
    if (!query.execute())
        return false;
    query = connection.query(
                "CREATE TABLE communities (temp INT)");
    if (!query.execute())
        return false;
    return true;
}

} /* namespace database */
} /* namespace accord */
