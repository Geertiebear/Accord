#ifndef ACCORD_DATABASE_DATABASE_H
#define ACCORD_DATABASE_DATABASE_H

#include <string>
#include <mysql.h>
#include <memory>
#include <boost/optional.hpp>

#include <accordshared/types/Database.h>
#include <accordshared/types/Request.h>

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

template<typename T>
static std::enable_if_t<std::is_same<T, uint64_t>::value, T> get(
        const char *data, unsigned long length = 0)
{
    (void) length;
    return std::stoull(std::string(data));
}

template<typename T>
static std::enable_if_t<std::is_same<T, int>::value, T> get(
        const char *data, unsigned long length = 0)
{
    (void) length;
    return std::stoi(std::string(data));
}

template<typename T>
static std::enable_if_t<std::is_same<T, std::string>::value, T> get(
        const char *data, unsigned long length = 0)
{
    (void) length;
    return std::string(data);
}

template<typename T>
static std::enable_if_t<std::is_same<T, std::vector<char>>::value, T> get(
        const char *data, unsigned long length = 0)
{
    return std::vector<char>(data, data + length);
}

struct TableUsers {
    uint64_t id;
    std::string name;
    std::vector<char> profilepic;
    int friends;
    int communities;
    std::string email;
    std::string password;
    std::string salt;

    TableUsers() = default;
    TableUsers(uint64_t id, const std::string &name,
               const std::vector<char> &profilepic, int friends,
               int communities, const std::string &email,
               const std::string &password, const std::string &salt) :
        id(id), name(name), profilepic(profilepic), friends(friends),
        communities(communities), email(email), password(password), salt(salt)
    { }

    TableUsers (MYSQL_ROW row, unsigned long *lengths) :
        id(get<uint64_t>(row[0])),
        name(get<std::string>(row[1])),
        profilepic(get<std::vector<char>>(row[2], lengths[2])),
        friends(get<int>(row[3])),
        communities(get<int>(row[4])),
        email(get<std::string>(row[5])),
        password(get<std::string>(row[6])),
        salt(get<std::string>(row[7]))
    { }

    std::vector<char> insertList(MYSQL *mysql);

    static constexpr unsigned int numFields = 8;
    static const std::string tableName;
};

struct TableChannels {
    uint64_t id;
    uint64_t community;
    std::string name;
    std::string description;

    TableChannels() = default;

    TableChannels(uint64_t id, uint64_t community,
                  const std::string &name,
                  const std::string &description) :
        id(id), community(community), name(name), description(description)
    {}

    TableChannels (MYSQL_ROW row, unsigned long *lengths) :
        id(get<uint64_t>(row[0])),
        community(get<uint64_t>(row[1])),
        name(get<std::string>(row[2])),
        description(get<std::string>(row[3]))
    { (void) lengths; }

    std::vector<char> insertList(MYSQL *mysql);

    types::ChannelsTable toShared()
    {
        return types::ChannelsTable(id, community, name, description);
    }

    static constexpr unsigned int numFields = 4;
    static const std::string tableName;
};

struct TableCommunities {
    uint64_t id;
    std::string name;
    std::vector<char> profilepic;
    int members;
    int channels;

    TableCommunities() = default;

    TableCommunities(uint64_t id, const std::string &name,
                     const std::vector<char> &profilepic,
                     int members, int channels) :
        id(id), name(name), profilepic(profilepic),
        members(members), channels(channels)
    { }

    TableCommunities (MYSQL_ROW row, unsigned long *lengths) :
        id(get<uint64_t>(row[0])),
        name(get<std::string>(row[1])),
        profilepic(get<std::vector<char>>(row[2], lengths[2])),
        members(get<int>(row[3])),
        channels(get<int>(row[4]))
    { }

    std::vector<char> insertList(MYSQL *mysql);

    types::CommunitiesTable toShared()
    {
        return types::CommunitiesTable(id, name, profilepic, members, channels);
    }

    static constexpr unsigned int numFields = 5;
    static const std::string tableName;
};

struct TableMessages {
    uint64_t id;
    uint64_t channel;
    uint64_t sender;
    std::string contents;
    uint64_t timestamp;

    TableMessages() = default;

    TableMessages(uint64_t id, uint64_t channel, uint64_t sender,
                  const std::string &contents, uint64_t timestamp) :
        id(id), channel(channel), sender(sender), contents(contents),
        timestamp(timestamp)
    { }

    TableMessages (MYSQL_ROW row, unsigned long *lengths) :
        id(get<uint64_t>(row[0])),
        channel(get<uint64_t>(row[1])),
        sender(get<uint64_t>(row[2])),
        contents(get<std::string>(row[3])),
        timestamp(get<uint64_t>(row[4]))
    { (void) lengths; }

    std::vector<char> insertList(MYSQL *mysql);

    types::MessagesTable toShared()
    {
        return types::MessagesTable(id, channel, sender, contents, timestamp);
    }

    static constexpr unsigned int numFields = 5;
    static const std::string tableName;
};

struct TableFriends {
    uint64_t id;
    uint64_t user1;
    uint64_t user2;
    std::string status;

    TableFriends() = default;

    TableFriends(uint64_t id, uint64_t user1, uint64_t user2,
                 const std::string &status) : id(id), user1(user1),
        user2(user2), status(status)
    { }

    TableFriends(MYSQL_ROW row, unsigned long *lengths) :
        id(get<uint64_t>(row[0])),
        user1(get<uint64_t>(row[1])),
        user2(get<uint64_t>(row[2])),
        status(get<std::string>(row[3]))
    { (void) lengths; }

    std::vector<char> insertList(MYSQL *mysql);

    static constexpr unsigned int numFields = 4;
    static const std::string tableName;
};

struct TableCommunityMembers {
    uint64_t id;
    uint64_t user;

    TableCommunityMembers() = default;

    TableCommunityMembers(uint64_t id, uint64_t user) : id(id),
        user(user)
    { }

    TableCommunityMembers(MYSQL_ROW row, unsigned long *lengths)
        : id(get<uint64_t>(row[0])),
          user(get<uint64_t>(row[1]))
    { (void) lengths; }

    std::vector<char> insertList(MYSQL *mysql);

    static constexpr unsigned int numFields = 2;
    static const std::string tableName;
};

struct TableChannelMembers {
    uint64_t id;
    uint64_t user;

    TableChannelMembers() = default;

    TableChannelMembers(uint64_t id, uint64_t user) : id(id),
        user(user)
    { }

    TableChannelMembers(MYSQL_ROW row, unsigned long *lengths)
        : id(get<uint64_t>(row[0])),
          user(get<uint64_t>(row[1]))
    { (void) lengths; }

    std::vector<char> insertList(MYSQL *mysql);

    static constexpr unsigned int numFields = 2;
    static const std::string tableName;
};

class Result {
public:
    Result(MYSQL_RES *res) : res(res) { }
    ~Result() {
        mysql_free_result(res);
    }

    template<typename T>
    std::vector<T> store()
    {
        std::vector<T> store;
        if (T::numFields != mysql_num_fields(res))
            return store;
        unsigned int numRows = mysql_num_rows(res);
        store.resize(numRows);
        for (unsigned int i = 0; i < numRows; i++) {
            MYSQL_ROW row = mysql_fetch_row(res);
            unsigned long *lengths = mysql_fetch_lengths(res);
            store[i] = T(row, lengths);
        }
        return store;
    }
    MYSQL_RES *res;
};

class Database {
public:
    Database(const DatabaseOptions &options);
    ~Database();

    int connect();
    void disconnect();
    bool verify();
    bool initDatabase();

    template<typename T>
    bool insert(T &row) {
        const std::string begin = "INSERT INTO " + T::tableName + " VALUES(";
        const auto middle = row.insertList(mysql);
        const std::string end = ")";
        std::vector<char> statement;

        std::copy(begin.begin(), begin.end(), std::back_inserter(statement));
        std::copy(middle.begin(), middle.end(), std::back_inserter(statement));
        std::copy(end.begin(), end.end(), std::back_inserter(statement));

        return !mysql_real_query(mysql, statement.data(), statement.size());
    }

    Result query(std::string query);
    bool initUser(uint64_t id, const std::string &name,
                  const std::string &email,
                  const std::string &password,
                  const std::string &salt);
    boost::optional<TableCommunities> initCommunity(uint64_t id, uint64_t user,
                                            const types::AddCommunity &request);
    boost::optional<TableChannels> initChannel(uint64_t id,
                                            const types::AddChannel &request);
    boost::optional<TableMessages> initMessage(uint64_t id, uint64_t channel,
                     uint64_t sender, const std::string &msg,
                     uint64_t timestamp);
    boost::optional<TableMessages> submitMessage(uint64_t channel,
                                                 uint64_t sender,
                                                 const std::string &msg,
                                                 uint64_t timestamp);
    bool addMember(uint64_t id, uint64_t user);
    bool addChannel(uint64_t id);
    bool sendFriendRequest(uint64_t from, uint64_t to);
    bool acceptFriendRequest(uint64_t id);
    bool isUserInCommunity(uint64_t userId, uint64_t communityId);
    bool canUserViewChannel(uint64_t userId, uint64_t channelId);
    boost::optional<TableUsers> getUser(const std::string &login);
    boost::optional<TableUsers> getUser(uint64_t id);
    boost::optional<TableChannels> getChannel(uint64_t id);
    boost::optional<TableCommunities> getCommunity(uint64_t id);
    boost::optional<TableMessages> getMessage(uint64_t id);
    std::vector<TableCommunities> getCommunitiesForUser(uint64_t id);
    std::vector<TableChannels> getChannelsForCommunity(uint64_t id);
    std::vector<TableMessages> getMessagesForChannel(uint64_t id);
    std::vector<TableChannels> getChannelsForUser(uint64_t id);
    std::vector<TableUsers> getUsersForChannel(uint64_t id);
    std::vector<TableUsers> getUsersForCommunity(uint64_t id);

    //helper functions
    static types::CommunitiesTable communityServerToShared(TableCommunities community);
    static types::ChannelsTable channelServerToShared(TableChannels channel);
    static types::MessagesTable messageServerToShared(TableMessages message);

private:
    const DatabaseOptions &options;
    MYSQL *mysql;
    bool connected = false;
};

} /* namespace database */
} /* namespace accord */

#endif
