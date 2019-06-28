#include <accordserver/database/Database.h>
#include <accordserver/log/Logger.h>
#include <accordserver/util/CryptoUtil.h>
#include <accordshared/types/Permissions.h>

#include <boost/variant.hpp>

namespace accord {
namespace database {

const std::string TableUsers::tableName = "users";
const std::string TableChannels::tableName = "channels";
const std::string TableCommunities::tableName = "communities";
const std::string TableMessages::tableName = "messages";
const std::string TableFriends::tableName = "friends";
const std::string TableCommunityMembers::tableName = "community_members";
const std::string TableChannelMembers::tableName = "channel_members";
const std::string TableUserRoles::tableName = "user_roles";
const std::string TableRoles::tableName = "roles";
const std::string TableChannelRoles::tableName = "channel_roles";
const std::string TableCommunityPermissions::tableName =
        "community_permissions";



using EscpPrintfArg = boost::variant<uint64_t, uint32_t, std::string, char,
        int, std::vector<char>>;

static std::vector<char> escaped_printf_vector_va(MYSQL *mysql,
                                                  std::string stmt,
                                                  const EscpPrintfArg *args)
{
    std::vector<char> res;
    for (auto it = stmt.begin(); it != stmt.end(); it++) {
        if (*it != '%') {
            res.push_back(*it);
            continue;
        }

        if (*it == '%') {
                it++;
                switch (*it) {
                case 's': {
                    /* escape the string */
                    const auto string = boost::get<std::string>(*args++);
                    char *escaped_string = new char[(string.length() * 2) + 1];
                    mysql_real_escape_string(mysql, escaped_string,
                                               string.c_str(), string.length());
                    const auto escaped_string_std = std::string(escaped_string);
                    delete escaped_string;
                    std::copy(escaped_string_std.begin(),
                              escaped_string_std.end(),
                              std::back_inserter(res));
                    break;
                }
                case 'c': {
                    if (*(it += 1) == 'v') {
                        const auto vector = boost::get<std::vector<
                                char>>(*args++);
                        char *escaped = new char[(vector.size() * 2) + 1];
                        int bytes = mysql_real_escape_string(mysql,
                                                                   escaped,
                                                       vector.data(),
                                                       vector.size());
                        const auto escaped_vector = std::vector<char>(
                                    escaped,escaped + bytes);
                        delete escaped;
                        std::copy(escaped_vector.begin(),
                                  escaped_vector.end(),
                                  std::back_inserter(res));
                        break;
                    }
                }
                case '%':
                    res.push_back(*it);
                    break;
                case 'u': {
                    if (*(it += 1) == 'l') {
                        const auto string = std::to_string(
                                    boost::get<uint64_t>(*args++));
                        std::copy(string.begin(), string.end(),
                                  std::back_inserter(res));
                        break;
                    }
                    const auto string = std::to_string(
                                    boost::get<uint32_t>(*args++));
                    std::copy(string.begin(), string.end(),
                              std::back_inserter(res));
                    break;
                }
                case 'i': {
                    const auto string = std::to_string(
                                boost::get<int>(*args++));
                    std::copy(string.begin(), string.end(),
                              std::back_inserter(res));
                    break;
                }
            }
        }
    }
    return res;
}

template <typename... Args>
static std::string escaped_printf(MYSQL *mysql, std::string stmt,
                                  const Args&... args)
{
    EscpPrintfArg argArray[] = {args...};
    const auto res = escaped_printf_vector_va(mysql, stmt, argArray);
    return std::string(res.begin(), res.end());
}

template <typename... Args>
static std::vector<char> escaped_printf_vector(MYSQL *mysql, std::string stmt,
                                               const Args&... args)
{
    EscpPrintfArg argArray[] = {args...};
    const auto res = escaped_printf_vector_va(mysql, stmt, argArray);
    return res;
}

std::vector<char> TableUsers::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%s', '%cv', '%i', '%i', '%s',"
                                        " '%s', '%s'",
                                 id, name, profilepic, friends, communities,
                                 email, password, salt);
}

std::vector<char> TableChannels::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul', '%s', '%s'", id,
                                 community, name, description);
}

std::vector<char> TableCommunities::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%s', '%cv', '%i', '%i'",
                                 id, name, profilepic, members, channels);
}

std::vector<char> TableMessages::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul', '%ul', '%s', '%ul'",
                                 id, channel, sender, contents, timestamp);
}

std::vector<char> TableChannelMembers::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul'",
                                 id, user);
}

std::vector<char> TableCommunityMembers::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul'",
                                 id, user);
}

std::vector<char> TableFriends::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul', '%ul', '%s'",
                                 id, user1, user2, status);
}

std::vector<char> TableUserRoles::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul'",
                                 id, user);
}

std::vector<char> TableRoles::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul', '%i', '%s', '%s'",
                                 id, community, position, name, colour);
}

std::vector<char> TableChannelRoles::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%ul', '%i', '%i'", id,
                                 channel, permission, allow);
}

std::vector<char> TableCommunityPermissions::insertList(MYSQL *mysql)
{
    return escaped_printf_vector(mysql, "'%ul', '%i', '%i'", id,
                                 permission, allow);
}

Database::Database(const DatabaseOptions &options) : options(options)
{ }

Database::~Database()
{
    if (connected)
        mysql_close(mysql);
}

int Database::connect()
{
    mysql = mysql_init(nullptr);
    if (!mysql_real_connect(mysql, options.address.c_str(),
                            options.user.c_str(),
                            options.password.c_str(), options.name.c_str(),
                            static_cast<unsigned int>(options.port),
                            nullptr, 0)) {
		log::Logger::log(log::FATAL, "Error connecting to database!");
		const char *error = mysql_error(mysql);
		if (error) {
            const auto stdError = std::string(error);
			log::Logger::log(log::FATAL, "MYSQL error: " + stdError);
		}
        return 0;
    }

    my_bool reconnect = 1;
    if (mysql_options(mysql, MYSQL_OPT_RECONNECT, &reconnect)) {
        log::Logger::log(log::FATAL, "Error setting reconnect option!");
        const char *error = mysql_error(mysql);
        if (error) {
            const auto stdError = std::string(error);
            log::Logger::log(log::FATAL, "MYSQL error: " + stdError);
        }
        return 0;
    }

    connected = true;
    return 1;
}

void Database::disconnect()
{
    mysql_close(mysql);
    connected = false;
}

static bool tableExists(MYSQL *mysql, const std::string &tableName)
{
    const std::string query = "SHOW TABLES LIKE '" + tableName + "'";
    if (mysql_real_query(mysql, query.c_str(), query.length()))
        return false;
    MYSQL_RES *res = mysql_store_result(mysql);
    if (!res)
        return false;
    if (!mysql_num_rows(res)) {
        mysql_free_result(res);
        return false;
    }
    mysql_free_result(res);
    return true;
}

bool Database::verify()
{
    if (!tableExists(mysql, "users"))
        return false;
    if (!tableExists(mysql, "communities"))
        return false;
    if (!tableExists(mysql, "friends"))
        return false;
    if (!tableExists(mysql, "community_members"))
        return false;
    if (!tableExists(mysql, "channels"))
        return false;
    if (!tableExists(mysql, "channel_members"))
        return false;
    if (!tableExists(mysql, "messages"))
        return false;
    if (!tableExists(mysql, "user_roles"))
        return false;
    if (!tableExists(mysql, "roles"))
        return false;
    if (!tableExists(mysql, "community_roles"))
        return false;
    if (!tableExists(mysql, "channel_roles"))
        return false;
    return true;
}

bool Database::initDatabase()
{
    bool success = true;
    const std::string users = "CREATE TABLE users (id BIGINT UNSIGNED, "
                                "name VARCHAR(255), profilepic "
                                "BLOB, friends INT,"
                                "communities INT, email VARCHAR(255),"
                                "password VARCHAR(255), salt VARCHAR(255))";
    if (mysql_real_query(mysql, users.c_str(), users.length()))
        success = false;
    const std::string communties = "CREATE TABLE communities "
                                   "(id BIGINT UNSIGNED,"
                                   "name VARCHAR(255), profilepic "
                                   "BLOB,"
                                   "members INT, channels INT)";
    if (mysql_real_query(mysql, communties.c_str(), communties.length()))
        success = false;
    const std::string friends = "CREATE TABLE friends (id BIGINT UNSIGNED, "
                                "user1 BIGINT UNSIGNED, user2 BIGINT UNSIGNED,"
                                "status ENUM('pending', 'accepted'))";
    if (mysql_real_query(mysql, friends.c_str(), friends.length()))
        success = false;

    const std::string communityMembers = "CREATE TABLE community_members "
                                            "(id BIGINT UNSIGNED, "
                                            "user BIGINT UNSIGNED)";
    if (mysql_real_query(mysql, communityMembers.c_str(),
                          communityMembers.length()))
        success = false;

    const std::string channels = "CREATE TABLE channels (id BIGINT UNSIGNED, "
                    "community BIGINT UNSIGNED, name VARCHAR(255),"
                    "description VARCHAR(255))";
    if (mysql_real_query(mysql, channels.c_str(), channels.length()))
        success = false;

    const std::string channelMembers = "CREATE TABLE channel_members "
                                       "(id BIGINT UNSIGNED, "
                                       "user BIGINT UNSIGNED)";
    if (mysql_real_query(mysql, channelMembers.c_str(),
                          channelMembers.length()))
        success = false;

    const std::string messages = "CREATE TABLE messages (id BIGINT UNSIGNED, "
                    "channel BIGINT UNSIGNED, sender BIGINT UNSIGNED,"
                    " contents VARCHAR(2000),"
                    "timestamp BIGINT UNSIGNED)";
    if (mysql_real_query(mysql, messages.c_str(), messages.length()))
        success = false;

    /* stores what users have what roles */
    const std::string userRoles = "CREATE TABLE user_roles (id BIGINT UNSIGNED,"
                    " user BIGINT UNSIGNED)";
    if (mysql_real_query(mysql, userRoles.c_str(), userRoles.length()))
        success = false;

    /*
     * Roles have permissions for communities and channels.
     * The allow flag is -1, 0, 1 for disallow, default and allow respectively.
     * channelRoles inherit permissions from @everyone
     * maybe in the future have a communitiesRoles inherit system as well?
     */
    const std::string roles = "CREATE TABLE roles (id BIGINT UNSIGNED,"
                    " community BIGINT UNSIGNED, position INT,"
                              " name VARCHAR(255), colour CHAR(6))";
    if (mysql_real_query(mysql, roles.c_str(), roles.length()))
        success = false;

    const std::string channelRoles = "CREATE TABLE channel_roles (id BIGINT "
                                     "UNSIGNED, channel BIGINT UNSIGNED, "
                                     "permission INT, allow INT)";
    if (mysql_real_query(mysql, channelRoles.c_str(), channelRoles.length()))
        success = false;

    const std::string communityRoles = "CREATE TABLE community_roles (id"
                                       " BIGINT UNSIGNED, permission INT, "
                                       "allow INT)";
    if (mysql_real_query(mysql, communityRoles.c_str(),
                         communityRoles.length()))
        success = false;

    return success;
}

Result Database::query(std::string query) {
    if (mysql_real_query(mysql, query.c_str(), query.length()))
        goto error;

    {
        MYSQL_RES *res = mysql_store_result(mysql);
        if (!res)
            goto error;

        return Result(res);
    }

error:
    const char *errorString = mysql_error(mysql);
    if (!errorString[0]) {
        log::Logger::log(log::FATAL, "Error executing query " + query +
                                     " without mysql error. Maybe the query does"
                                     " not return a result set...");
    } else {
        const auto stdErrorString = std::string(errorString);
        log::Logger::log(log::FATAL, "Error executuing query " + query +
                                     " with mysql error " + stdErrorString);
    }
    return Result(nullptr);
}

bool Database::initUser(uint64_t id, const std::string &name,
                        const std::string &email,
                        const std::string &password,
                        const std::string &salt)
{

    TableUsers user(id, name, std::vector<char>(), 0, 0, email, password, salt);
    return insert(user);
}

boost::optional<TableCommunities> Database::initCommunity(uint64_t id,
                                                          uint64_t user,
                             const types::AddCommunity &request)
{
    auto check = getCommunity(id);
    if (check)
        return boost::none;

    TableCommunities community(id, request.name,
                          request.profilepic, 0, 0);
    if(!insert(community))
        return boost::none;
    if(!addMember(id, user))
        return boost::none;

    return community;
}

boost::optional<TableChannels> Database::initChannel(uint64_t id,
                                            const types::AddChannel &request)
{
    auto check = getChannel(id);
    if (check)
        return boost::none;

    TableChannels channel(id, request.community, request.name,
                          request.description);
    if (!insert(channel))
        return boost::none;
    if (!addChannel(request.community))
        return boost::none;

    auto users = getUsersForCommunity(request.community);
    for (auto user : users) {
        TableChannelMembers channelMember(id, user.id);
        if(!insert(channelMember))
            return boost::none;
    }
    return channel;
}

boost::optional<TableMessages> Database::initMessage(uint64_t id,
                                                     uint64_t channel,
                                                     uint64_t sender,
                                                     const std::string &msg,
                                                     uint64_t timestamp)
{
    auto check = getMessage(id);
    if (check) {
        log::Logger::log(log::WARNING, "Message already exists!");
        return boost::none;
    }

    TableMessages message(id, channel, sender, msg, timestamp);
    if (!insert(message))
        return boost::none;
    return message;
}

boost::optional<TableMessages> Database::submitMessage(uint64_t channel,
                                                       uint64_t sender,
                                                       const std::string &msg,
                                                       uint64_t timestamp)
{
    auto id = util::CryptoUtil::getRandomUINT64();
    return initMessage(id, channel, sender, msg, timestamp);
}

boost::optional<TableRoles> Database::initRole(uint64_t id, uint64_t community,
                                               int position,
                                               const std::string &name,
                                               const std::string &colour)
{
    auto check = getRole(id);
    if (check) {
        log::Logger::log(log::WARNING, "Role already exists!");
        return boost::none;
    }

    TableRoles role(id, community, position, name, colour);
    if (!insert(role))
        return boost::none;
    return role;
}

bool Database::addMember(uint64_t id, uint64_t user)
{
    auto community = getCommunity(id);
    if (!community)
        return false;
    const std::string memberUpdate = "UPDATE communities SET members='" +
            std::to_string(community.get().members + 1) + "' WHERE id='" +
            std::to_string(community.get().id) + "'";
    if(mysql_real_query(mysql, memberUpdate.c_str(), memberUpdate.length()))
        return false;

    auto userTable = getUser(user);
    if (!userTable)
        return false;
    const std::string userUpdate = "UPDATE users SET communities='" +
            std::to_string(userTable.get().communities + 1) + "' WHERE id='" +
            std::to_string(userTable.get().id) + "'";
	log::Logger::log(log::DEBUG, userUpdate);
    if (mysql_real_query(mysql, userUpdate.c_str(), userUpdate.length()))
        return false;

    TableCommunityMembers communityMember(id, user);
    if (!insert(communityMember))
        return false;

    auto channels = getChannelsForCommunity(id);
    for (auto &channel : channels) {
        TableChannelMembers channelMember(channel.id, user);
        if(!insert(channelMember))
            return false;
    }

    return true;
}

bool Database::addChannel(uint64_t id)
{
    auto community = getCommunity(id);
    if (!community)
        return false;
    const std::string update = "UPDATE communities SET channels='" +
            std::to_string(community.get().channels + 1) + "' WHERE id='" +
            std::to_string(community.get().id) + "'";
    return !mysql_real_query(mysql, update.c_str(), update.length());
}

bool Database::addUserRole(uint64_t user, uint64_t role)
{
    /* TODO: check if the user already has the role */
    TableUserRoles userRole(role, user);
    if (!insert(userRole))
        return false;
    return true;
}

bool Database::addCommunityPermission(uint64_t role, types::CommunityPermissions
                                      permission, int allow)
{
    /* TODO: check if the role already has the permission */
    TableCommunityPermissions communityPermission(role,
                                                  static_cast<int>(permission),
                                                  allow);
    if (!insert(communityPermission))
        return false;
    return true;
}

bool Database::addChannelPermission(uint64_t role, uint64_t channel,
                                    types::ChannelPermissions permission,
                                    int allow)
{
    /* TODO: same deal as above */
    TableChannelRoles channelPermission(role, channel,
                                        static_cast<int>(permission),
                                        allow);
    if (!insert(channelPermission))
        return false;
    return true;
}

bool Database::sendFriendRequest(uint64_t from, uint64_t to)
{
    uint64_t id = util::CryptoUtil::getRandomUINT64();
    TableFriends request(id, from, to, "pending");
    return insert(request);
}

bool Database::acceptFriendRequest(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM friends WHERE"
                                            " id='%ul'");
    Result result = query(statement);
    auto res = result.store<TableFriends>();
    if (res.size() != 1) {
        //what
        log::Logger::log(log::WARNING, "Friend request id " +
                         std::to_string(id) + " has multiple entries..?");
        return false;
    }

    const std::string update = "UPDATE 'friends' SET 'status'='accepted' WHERE"
                        " id='" + std::to_string(res[0].id) + "'";
    return mysql_real_query(mysql, update.c_str(), update.length());
}

bool Database::isUserInCommunity(uint64_t userId, uint64_t communityId)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM "
                                                 "community_members WHERE "
                                                 "id='%ul' AND user='%ul'",
                                          communityId, userId);
    Result result = query(statement);
    const auto res = result.store<TableCommunityMembers>();
    if (res.size() > 1) {
        /* there is something wrong with the database,
         * assume that the user is in there
         * and report a warning. This is non-fatal
         * no need to crash the server or inconvencience
         * the client
         */
        log::Logger::log(log::WARNING, "There are multiple entries"
                                       "for a user in community_members."
                                       "PLEASE CHECK!");
        return true;
    }
    if (res.size() == 0)
        return false;
    return true;
}

bool Database::isUserInChannel(uint64_t userId, uint64_t channelId)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM "
                                                 "community_members WHERE id="
                                                 "(SELECT community FROM "
                                                 "channels WHERE id='%ul') AND "
                                                 "user='%ul'", channelId,
                                          userId);
    Result result = query(statement);
    const auto res = result.store<TableCommunityMembers>();
    if (res.size() > 1) {
        /* same deal as above */
        log::Logger::log(log::WARNING, "There are multiple entries"
                                       "for a user in community_members."
                                       "PLEASE CHECK!");
        return true;
    }
    if (res.size() == 0)
        return false;
    return true;
}

bool Database::hasCommunityPermission(uint64_t user, uint64_t community,
                            types::CommunityPermissions permission)
{
    const auto statement = escaped_printf(mysql, "SELECT id FROM "
                                                 "community_roles INNER JOIN "
                                                 "roles ON community_roles.id "
                                                 "= roles.id WHERE user='%ul' "
                                                 "AND community='%ul' AND "
                                                 "permission='%ul'", user,
                                          community,
                                          static_cast<uint64_t>(permission));
    std::vector<uint64_t> ids;
    Result result = query(statement);
    result.process([&ids](MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        auto id = std::stoull(row[0]);
        ids.push_back(id);
    });

    if (ids.size() == 0)
        return false;
    return true;
}

bool Database::hasChannelPermission(uint64_t user, uint64_t channel,
                          types::ChannelPermissions permission)
{
    const auto statement = escaped_printf(mysql, "SELECT id FROM "
                                                 "channel_roles INNER JOIN "
                                                 "roles ON channel_roles.id "
                                                 "= roles.id WHERE user='%ul' "
                                                 "AND channel='%ul' AND "
                                                 "permission='%ul'", user,
                                          channel,
                                          static_cast<uint64_t>(permission));
    std::vector<uint64_t> ids;
    Result result = query(statement);
    result.process([&ids](MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        auto id = std::stoull(row[0]);
        ids.push_back(id);
    });

    if (ids.size() == 0)
        return false;
    return true;
}

boost::optional<TableUsers> Database::getUser(const std::string &login)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM users WHERE"
                                                  " name='%s' OR email='%s'",
                                           login, login);
    Result result = query(statement);
    const auto res = result.store<TableUsers>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Login " + login + "has multiple "
                                                            "entries!");
        }
        return boost::none;
    }
    return res[0];
}

boost::optional<TableUsers> Database::getUser(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM users WHERE"
                                                 " id='%ul'", id);
    Result result = query(statement);
    const auto res = result.store<TableUsers>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "User " + std::to_string(id) +
                             " has multiple entries!");
        }
        return boost::none;
    }
    return res[0];
}

boost::optional<TableChannels> Database::getChannel(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM channels WHERE "
                                                 "id='%ul'", id);
    Result result = query(statement);
    const auto res = result.store<TableChannels>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Channel " + std::to_string(id) +
                             " has multiple entries!");
        }
        return boost::none;
    }
    return res[0];
}

boost::optional<TableCommunities> Database::getCommunity(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM communities "
                                                 "WHERE id='%ul'", id);
    Result result = query(statement);
    const auto res = result.store<TableCommunities>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Community " + std::to_string(id) +
                             " has multiple entries!");
        }
        return boost::none;
    }
	log::Logger::log(log::DEBUG, "Got a community!");
    return res[0];
}

boost::optional<TableMessages> Database::getMessage(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM messages WHERE"
                                                 " id='%ul'", id);
    Result result = query(statement);
    const auto res = result.store<TableMessages>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Message " + std::to_string(id) +
                             " has multiple entries!");
        }
        return boost::none;
    }
    return res[0];
}

boost::optional<TableCommunities> Database::getCommunityForChannel(uint64_t
                                                                   channel)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM communities "
                                                 "WHERE id IN (SELECT "
                                                 "channels.community FROM "
                                                 "channels WHERE id='%ul')",
                                          channel);
    Result result = query(statement);
    const auto res = result.store<TableCommunities>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Channel " + std::to_string(channel) +
                             " has multiple communities!");
        }
        return boost::none;
    }
    return res[0];
}

boost::optional<TableRoles> Database::getRole(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM roles "
                                                 "WHERE id='%ul'", id);
    Result result = query(statement);
    const auto res = result.store<TableRoles>();
    if (res.size() != 1) {
        if (res.size() > 1) {
            log::Logger::log(log::FATAL, "Role id " + std::to_string(id) +
                             " has multiple entries!");
        }
        return boost::none;
    }
    return res[0];
}

std::vector<TableCommunities> Database::getCommunitiesForUser(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM communities "
                                                 "WHERE id IN (SELECT "
                                                 "community_members.id FROM "
                                                 "community_members WHERE user="
                                                 "'%ul')", id);
    Result result = query(statement);
    return result.store<TableCommunities>();
}

std::vector<TableChannels> Database::getChannelsForCommunity(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM channels where"
                                                 " community='%ul'", id);
    Result result = query(statement);
    return result.store<TableChannels>();
}

std::vector<TableChannels> Database::getChannelsForUser(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM channels "
                                                 "WHERE id IN (SELECT "
                                                 "channel_members.id FROM "
                                                 "channel_members WHERE user="
                                                 "'%ul')", id);
    Result result = query(statement);
    return result.store<TableChannels>();
}

std::vector<TableMessages> Database::getMessagesForChannel(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM messages "
                                                 "WHERE channel='%ul'", id);
    Result result = query(statement);
    return result.store<TableMessages>();
}

/*
 * currently unused, maybe useful sometime in the future?
*/
std::vector<TableUsers> Database::getUsersForChannel(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM users WHERE "
                                                 "id IN (SELECT "
                                                 "channel_members.user FROM "
                                                 "channel_members WHERE id="
                                                 "'%ul')", id);
    Result result = query(statement);
    return result.store<TableUsers>();
}

std::vector<TableUsers> Database::getUsersForCommunity(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM users WHERE id "
                                                 "IN (SELECT "
                                                 "community_members.user FROM "
                                                 "community_members WHERE id="
                                                 "'%ul')", id);
    Result result = query(statement);
    return result.store<TableUsers>();
}

std::vector<TableRoles> Database::getRolesForCommunity(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM roles WHERE "
                                                 "community='%ul'", id);
    Result result = query(statement);
    return result.store<TableRoles>();
}

std::vector<TableRoles> Database::getRolesForChannel(uint64_t id)
{
    const auto statement = escaped_printf(mysql, "SELECT * FROM roles WHERE "
                                                 "id IN (SELECT "
                                                 "channel_roles.id FROM"
                                                 " channel_roles WHERE"
                                                 " channel='%ul'", id);
    Result result = query(statement);
    return result.store<TableRoles>();
}

std::vector<types::CommunityPermission> Database::getCommunityPermissions(
        uint64_t role)
{
    const auto statement = escaped_printf(mysql, "SELECT permission, allow "
                                                 "FROM community_roles WHERE "
                                                 "id='%ul'", role);
    std::vector<types::CommunityPermission> ret;
    Result result = query(statement);
    result.process([&ret](MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        int permission = std::stoi(std::string(row[0]));
        int allow = std::stoi(std::string(row[1]));
        ret.emplace_back(static_cast<types::CommunityPermissions>(permission),
                         allow);
    });
    return ret;
}

std::vector<types::ChannelPermission> Database::getChannelPermissions(
        uint64_t role)
{
    const auto statement = escaped_printf(mysql, "SELECT permission, allow "
                                                 "FROM channel_roles WHERE "
                                                 "id='%ul'", role);
    std::vector<types::ChannelPermission> ret;
    Result result = query(statement);
    result.process([&ret] (MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        int permission = std::stoi(std::string(row[0]));
        int allow = std::stoi(std::string(row[1]));
        ret.emplace_back(static_cast<types::ChannelPermissions>(permission),
                         allow);
    });
    return ret;
}

std::vector<types::CommunityPermission> Database::getCommunityPermissionsForUser(
        uint64_t user, uint64_t community)
{
    const auto statement = escaped_printf(mysql, "SELECT permission, allow "
                                                 "FROM community_permissions "
                                                 "WHERE id IN (SELECT id FROM "
                                                 "user_roles INNER JOIN roles "
                                                 "ON user_roles.id = roles.id "
                                                 "WHERE user='%ul' AND "
                                                 "community='%ul')",
                                          user, community);
    std::vector<types::CommunityPermission> ret;
    Result result = query(statement);
    result.process([&ret] (MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        int permission = get<int>(row[0]);
        int allow = get<int>(row[1]);
        ret.emplace_back(static_cast<types::CommunityPermissions>(permission),
                allow);
    });
    return ret;
}

std::vector<types::ChannelPermission> Database::getChannelPermissionsForUser(
        uint64_t user, uint64_t channel)
{
    const auto statement = escaped_printf(mysql, "SELECT permissions, allow "
                                                 "FROM channel_roles WHERE id "
                                                 "IN (SELECT id FROM "
                                                 "user_roles WHERE user='%ul') "
                                                 "AND channel='%ul'",
                                          user, channel);
    std::vector<types::ChannelPermission> ret;
    Result result = query(statement);
    result.process([&ret] (MYSQL_ROW row, unsigned long *lengths) {
        (void) lengths;
        int permission = get<int>(row[0]);
        int allow = get<int>(row[1]);
        ret.emplace_back(static_cast<types::ChannelPermissions>(permission),
                         allow);
    });
    return ret;
}

types::CommunitiesTable Database::communityServerToShared(TableCommunities community)
{
    return types::CommunitiesTable(community.id,
                                   community.name,
                                   community.profilepic,
                                   community.members,
                                   community.channels);
}

types::ChannelsTable Database::channelServerToShared(TableChannels channel)
{
    return types::ChannelsTable(channel.id,
                                channel.community,
                                channel.name,
                                channel.description);
}

types::MessagesTable Database::messageServerToShared(TableMessages message)
{
    return types::MessagesTable(message.id,
                                message.channel,
                                message.sender,
                                message.contents,
                                message.timestamp);
}

} /* namespace database */
} /* namespace accord */
