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

struct TableUsers {
    uint64_t id;
    std::string name;
    std::vector<char> profilepic;
    int friends;
    int communities;
    std::string email;
    std::string password;
    std::string salt;

    static TableUsers fromRow(MYSQL_ROW row, unsigned long *lengths)
    {
        TableUsers res;
        res.id = std::stoull(std::string(row[0]));
        res.name = std::string(row[1]);
        res.profilepic = std::vector<char>(row[2], row[2] + lengths[2]);
        res.friends = std::stoi(std::string(row[3]));
        res.communities = std::stoi(std::string(row[4]));
        res.email = std::string(row[5]);
        res.password = std::string(row[6]);
        res.salt = std::string(row[7]);
        return res;
    }

    static constexpr unsigned int numFields = 8;
};

struct TableChannels {
    uint64_t id;
    uint64_t community;
    std::string name;
    std::string description;

    static TableChannels fromRow(MYSQL_ROW row, unsigned long *lengths)
    {
        TableChannels res;
        (void) lengths;
        res.id = std::stoull(std::string(row[0]));
        res.community = std::stoull(std::string(row[1]));
        res.name = std::string(row[2]);
        res.description = std::string(row[3]);
        return res;
    }

    static constexpr unsigned int numFields = 4;
};

struct TableCommunities {
    uint64_t id;
    std::string name;
    std::vector<char> profilepic;
    int members;
    int channels;

    static TableCommunities fromRow(MYSQL_ROW row, unsigned long *lengths)
    {
        TableCommunities res;
        res.id = std::stoull(std::string(row[0]));
        res.name = std::string(row[1]);
        res.profilepic = std::vector<char>(row[2], row[2] + lengths[2]);
        res.members = std::stoull(row[3]);
        res.channels = std::stoull(row[4]);
        return res;
    }

    static constexpr unsigned int numFields = 5;
};

struct TableMessages {
    uint64_t id;
    uint64_t channel;
    uint64_t sender;
    std::string contents;
    uint64_t timestamp;

    static TableMessages fromRow(MYSQL_ROW row, unsigned long *lengths)
    {
        TableMessages res;
        res.id = std::stoull(std::string(row[0]));
        res.channel = std::stoull(std::string(row[1]));
        res.sender = std::stoull(std::string(row[2]));
        res.contents = std::string(row[3]);
        res.timestamp = std::stoull(std::string(row[4]));
    }

    static constexpr unsigned int numFields = 5;
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
        unsigned long *lengths = mysql_fetch_lengths(res);
        for (unsigned int i = 0; i < numRows; i++) {
            MYSQL_ROW row = mysql_fetch_row(res);
            store[i] = T::fromRow(row, lengths);
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
    Result query(std::string query);
    bool initUser(uint64_t id, const std::string &name,
                  const std::string &email,
                  const std::string &password,
                  const std::string &salt);
    /*bool initCommunity(uint64_t id, uint64_t user, const types::AddCommunity &request,
                       table_communities *ret = nullptr);
    bool initChannel(uint64_t id, const types::AddChannel &request,
                     table_channels *ret = nullptr);
    bool initMessage(uint64_t id, uint64_t channel,
                     uint64_t sender, const std::string &msg,
                     uint64_t timestamp, table_messages *ret = nullptr);
    bool submitMessage(uint64_t channel, uint64_t sender,
                       const std::string &msg, uint64_t timestamp,
                       table_messages *ret = nullptr);
    bool addMember(uint64_t id, uint64_t user);
    bool addChannel(uint64_t id);
    bool sendFriendRequest(uint64_t from, uint64_t to);
    bool acceptFriendRequest(uint64_t id);
    bool isUserInCommunity(uint64_t userId, uint64_t communityId);
    bool canUserViewChannel(uint64_t userId, uint64_t channelId);
    */
    boost::optional<TableUsers> getUser(const std::string &login);
    boost::optional<TableUsers> getUser(uint64_t id);
    boost::optional<TableChannels> getChannel(uint64_t id);
    boost::optional<TableCommunities> getCommunity(uint64_t id);
    boost::optional<TableMessages> getMessage(uint64_t id);
    /*
    std::vector<table_communities> getCommunitiesForUser(uint64_t id);
    std::vector<table_channels> getChannelsForCommunity(uint64_t id);
    std::vector<table_messages> getMessagesForChannel(uint64_t id);
    std::vector<table_channels> getChannelsForUser(uint64_t id);
    std::vector<table_users> getUsersForChannel(uint64_t id);
    std::vector<table_users> getUsersForCommunity(uint64_t id);
    mysqlpp::Query query(std::string statement); */

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
