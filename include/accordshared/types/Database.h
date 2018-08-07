#ifndef ACCORD_TYPES_DATABASE_H
#define ACCORD_TYPES_DATABASE_H

#include <accordshared/util/Serialization.h>
#include <boost/any.hpp>

namespace accord {
namespace types {

class CommunitiesTable : public util::Serializable {
public:
    CommunitiesTable() { }
    CommunitiesTable(uint64_t id, std::string name, std::string profilepic,
                     int members, int channels) : id(id), name(name),
        profilepic(profilepic), members(members), channels(channels) { }

    static CommunitiesTable *Creator()
    {
        return new CommunitiesTable();
    }

    uint64_t id;
    std::string name;
    std::string profilepic; //sql blob type
    int members;
    int channels;
    std::vector<char> serialize();
    boost::any deserialize(const std::vector<char> &data);
};

} /* namespace types */
} /* namespace accord */

#endif
