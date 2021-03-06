#ifndef ACCORD_TYPES_RETURN_H
#define ACCORD_TYPES_RETURN_H

#include <vector>
#include <ctime>
#include <list>

#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>

#include <accordshared/types/Database.h>

namespace accord {
namespace types {

class ChannelsReturn {
public:
    ChannelsReturn() { }
    ChannelsReturn(const std::vector<ChannelsTable> &channels,
                   uint64_t community)
        : channels(channels), community(community) { }

    std::vector<ChannelsTable> channels;
    uint64_t community;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(channels, community);
    }
};

class MessagesReturn {
public:
    MessagesReturn() { }
    MessagesReturn(const std::vector<MessagesTable> &messages,
                   uint64_t channel)
        : messages(messages), channel(channel) { }
    std::vector<MessagesTable> messages;
    uint64_t channel;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(messages, channel);
    }
};

class MessageSuccess {
public:
    MessageSuccess() { }
    MessageSuccess(uint64_t id) : id(id) { }
    uint64_t id;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(id);
    }
};

class Token {
public:
    Token() { }
    Token(const std::string &token, const std::string &key,
          uint64_t id, std::time_t expiration) : token(token), key(key), id(id),
        expiration(expiration) { }
    std::string token, key;
    uint64_t id;
    std::time_t expiration;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(token, key, id, expiration);
    }
};

/*
 * we dont return the entire user's column because it
 * has sensitive data like passwords. Instead we
 * return a subset of the data
 */
class UserData {
public:
    UserData() { }
    UserData(uint64_t id, const std::string &name,
             const std::vector<char> &profilepic) :
        id(id), name(name), profilepic(profilepic) { }
    uint64_t id;
    std::string name;
    std::vector<char> profilepic;

    bool operator== (const UserData &rhs) const
    {
        return rhs.id == id;
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(id, name, profilepic);
    }
};

class InviteRet {
public:
    InviteRet() { }
    InviteRet(uint64_t id, const std::string &invite) : id(id), invite(invite)
    { }
    uint64_t id;
    std::string invite;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(id, invite);
    }
};

class OnlineListRet {
public:
    OnlineListRet() { }
    OnlineListRet(std::list<types::UserData> list, uint64_t id) :
        list(list), id(id)
    { }
    std::list<types::UserData> list;
    uint64_t id;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(list, id);
    }
};

} /* namespace types */
} /* namespace accord */

#endif
