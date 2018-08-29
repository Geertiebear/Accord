#ifndef ACCORD_TYPES_RETURN_H
#define ACCORD_TYPES_RETURN_H

#include <vector>

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

} /* namespace types */
} /* namespace accord */

#endif
