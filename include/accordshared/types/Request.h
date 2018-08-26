#ifndef ACCORD_TYPES_REQUEST_H
#define ACCORD_TYPES_REQUEST_H

#include <string>
#include <vector>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace accord {
namespace types {

class AddCommunity {
public:
    AddCommunity() { }
    AddCommunity(std::string name, std::vector<char> profilepic)
        : name(name), profilepic(profilepic) { }
    std::string name;
    std::vector<char> profilepic;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(name, profilepic);
    }
};

class AddChannel {
public:
    AddChannel() { }
    AddChannel(uint64_t community, std::string name, std::string description)
        : community(community), name(name), description(description) { }
    uint64_t community;
    std::string name;
    std::string description;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(community, name, description);
    }
};

class Channels {
public:
    Channels() { }
    Channels(uint64_t community) : community(community) { }
    uint64_t community;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(community);
    }
};

} /* namespace types */
} /* namespace accord */

#endif
