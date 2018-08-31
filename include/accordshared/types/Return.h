#ifndef ACCORD_TYPES_RETURN_H
#define ACCORD_TYPES_RETURN_H

#include <vector>
#include <ctime>

#include <cereal/types/vector.hpp>

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

} /* namespace types */
} /* namespace accord */

#endif
