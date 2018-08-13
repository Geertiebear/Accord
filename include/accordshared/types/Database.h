#ifndef ACCORD_TYPES_DATABASE_H
#define ACCORD_TYPES_DATABASE_H

#include <cstdint>
#include <string>
#include <vector>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace accord {
namespace types {

class CommunitiesTable {
public:
    CommunitiesTable() { }
    CommunitiesTable(uint64_t id, std::string name, std::vector<char> profilepic,
                     int members, int channels) : id(id), name(name),
        profilepic(profilepic), members(members), channels(channels) { }

    uint64_t id;
    std::string name;
    std::vector<char> profilepic; //sql blob type
    int members;
    int channels;

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive(id, name, profilepic, members, channels);
    }
};

} /* namespace types */
} /* namespace accord */

#endif
