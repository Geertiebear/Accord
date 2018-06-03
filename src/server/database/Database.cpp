#include <accordserver/database/Database.h>
#include <accordserver/log/Logger.h>
#include <accordserver/util/CryptoUtil.h>

namespace accord {
namespace database {

sql_create_8(users, 1, 8,
             mysqlpp::sql_bigint_unsigned, id,
             mysqlpp::sql_varchar, name,
             mysqlpp::sql_blob_null, profilepic,
             mysqlpp::sql_int, friends,
             mysqlpp::sql_int, communities,
             mysqlpp::sql_varchar, email,
             mysqlpp::sql_varchar, password,
             mysqlpp::sql_varchar, salt);

sql_create_4(friends, 1, 4,
             mysqlpp::sql_bigint_unsigned, id,
             mysqlpp::sql_bigint_unsigned, user1,
             mysqlpp::sql_bigint_unsigned, user2,
             mysqlpp::sql_enum, status);

sql_create_5(communities, 1, 5,
             mysqlpp::sql_bigint_unsigned, id,
             mysqlpp::sql_varchar, name,
             mysqlpp::sql_blob_null, profilepic,
             mysqlpp::sql_int, members,
             mysqlpp::sql_int, channels);

sql_create_2(community_members, 1, 2,
             mysqlpp::sql_bigint_unsigned, id,
             mysqlpp::sql_bigint_unsigned, user);

table_users::table_users(std::shared_ptr<users> table) : table(table)
{}

mysqlpp::sql_bigint_unsigned& table_users::id()
{
    return table->id;
}

mysqlpp::sql_varchar& table_users::name()
{
    return table->name;
}
mysqlpp::sql_blob_null& table_users::profilepic()
{
    return table->profilepic;
}

mysqlpp::sql_int& table_users::friends()
{
    return table->friends;
}

mysqlpp::sql_int& table_users::communities()
{
    return table->communities;
}

mysqlpp::sql_varchar& table_users::email()
{
    return table->email;
}

mysqlpp::sql_varchar& table_users::password()
{
    return table->password;
}

mysqlpp::sql_varchar& table_users::salt()
{
    return table->salt;
}

table_friends::table_friends(std::shared_ptr<friends> table) : table(table) { }

mysqlpp::sql_bigint_unsigned& table_friends::id()
{
    return table->id;
}

mysqlpp::sql_bigint_unsigned& table_friends::user1()
{
    return table->user1;
}

mysqlpp::sql_bigint_unsigned& table_friends::user2()
{
    return table->user2;
}

mysqlpp::sql_enum& table_friends::status()
{
    return table->status;
}

table_communities::table_communities(std::shared_ptr<communities> table)
    : table(table) { }

mysqlpp::sql_bigint_unsigned& table_communities::id()
{
    return table->id;
}

mysqlpp::sql_varchar& table_communities::name()
{
    return table->name;
}

mysqlpp::sql_blob_null& table_communities::profilepic()
{
    return table->profilepic;
}

mysqlpp::sql_int& table_communities::members()
{
    return table->members;
}

mysqlpp::sql_int& table_communities::channels()
{
    return table->channels;
}

table_community_members::table_community_members(
        std::shared_ptr<community_members> table) : table(table) { }

mysqlpp::sql_bigint_unsigned& table_community_members::id()
{
    return table->id;
}

mysqlpp::sql_bigint_unsigned& table_community_members::user()
{
    return table->user;
}

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
    query = connection.query("SHOW TABLES LIKE 'community_members'");
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
                      "communities INT, email VARCHAR(255),"
                      "password VARCHAR(255), salt VARCHAR(255))");
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
        query = connection.query(
                "CREATE TABLE community_members (id BIGINT UNSIGNED, "
                "user BIGINT UNSIGNED)");
        if (!query.execute())
            return false;
    } catch (mysqlpp::BadQuery &e) {
        log::Logger::log(log::WARNING, "Bad query made in database init: " +
                         std::string(e.what()));
    }
    return true;
}

mysqlpp::Query Database::query(std::string statement)
{
    return connection.query(statement.c_str());
}

bool Database::initUser(uint64_t id, const std::string &name,
                        const std::string &email,
                        const std::string &password,
                        const std::string &salt)
{


    mysqlpp::Query query = connection.query();
    users user(id, name, mysqlpp::null, 0, 0, email, password, salt);
    query.insert(user);
    return query.execute();
}

bool Database::initCommunity(uint64_t id, uint64_t user, const std::string &name)
{
    table_communities check = getCommunity(id);
    if (check.table != NULL)
        return false;

    mysqlpp::Query query = connection.query();
    communities community(id, name, mysqlpp::null, 0, 0);
    query.insert(community);
    if(!query.execute())
        return false;
    if(!addMember(id, user))
        return false;
    return true;
}

bool Database::addMember(uint64_t id, uint64_t user)
{
    mysqlpp::Query query = connection.query();

    table_communities community = getCommunity(id);
    communities originalCommunity = *community.table;
    community.members()++;
    query.update(originalCommunity, *community.table);
    if(!query.execute())
        return false;

    table_users userTable = getUser(user);
    users originalUser = *userTable.table;
    userTable.communities()++;
    query.update(originalUser, *userTable.table);
    if (!query.execute())
        return false;

    community_members community_member(id, user);
    query.insert(community_member);

    return query.execute();
}

bool Database::sendFriendRequest(uint64_t from, uint64_t to)
{
    mysqlpp::Query query = connection.query();

    uint64_t id = util::CryptoUtil::getRandomUINT64();
    friends request(id, from, to, "pending");
    query.insert(request);
    return query.execute();
}

bool Database::acceptFriendRequest(uint64_t id)
{
    mysqlpp::Query query = connection.query("SELECT * FROM friends WHERE"
                                            " id=" + std::to_string(id));
    std::vector<friends> res;
    query.storein(res);
    if (res.size() != 1) {
        //what
        log::Logger::log(log::WARNING, "Friend request id " +
                         std::to_string(id) + " has multiple entries..?");
        return false;
    }
    friends request = res[0];
    friends original = res[0];
    request.status = "accepted";
    query.update(original, request);
    return query.execute();
}

table_users Database::getUser(const std::string &login,
                              const std::string &password)
{
    mysqlpp::Query query = connection.query("SELECT * FROM users WHERE"
                                          " name='" + login + "' OR"
                                          " email='" + login + "'");
    std::vector<users> res;
    query.storein(res);
    if (res.size() != 1) {
        log::Logger::log(log::WARNING, "Login " + login + " has multiple"
                                                          " entries!");
        return table_users(NULL);
    }
    auto user = std::make_shared<users>(res[0]);
    table_users table(user);
    return table;
}

table_users Database::getUser(uint64_t id)
{
    mysqlpp::Query query = connection.query("SELECT * FROM users WHERE"
                                          " id=" + std::to_string(id));
    std::vector<users> res;
    query.storein(res);
    if (res.size() != 1) {
        log::Logger::log(log::WARNING, "User Id " + std::to_string(id) +
                         " has multiple entries!");
        return table_users(NULL);
    }
    auto user = std::make_shared<users>(res[0]);
    table_users table(user);
    return table;
}

table_communities Database::getCommunity(uint64_t id)
{
    mysqlpp::Query query = connection.query("SELECT * FROM communities WHERE "
                                            "id=" + std::to_string(id));
    std::vector<communities> res;
    query.storein(res);
    if (res.size() != 1) {
        log::Logger::log(log::WARNING, "Community Id " + std::to_string(id) +
                         " has multiple entries!");
        return table_communities(NULL);
    }
    auto community = std::make_shared<communities>(res[0]);
    table_communities table(community);
    return table;
}

} /* namespace database */
} /* namespace accord */
