#ifndef ACCORD_NETWORK_REQUEST_DATA_PACKET_H
#define ACCORD_NETWORK_REQUEST_DATA_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

/* TODO: remove this packet - it is useless, everything can be done through
 * seriliazation packet */

enum RequestIds {
    COMMUNITIES_TABLE_REQUEST = 0,
    ADD_COMMUNITY_REQUEST = 1,
    CHANNELS_REQUEST = 2,
};

class RequestDataPacket : public Packet {
public:
    virtual RequestDataPacket *clone() const {
        return new RequestDataPacket();
    }

    int getId() const {
        return REQUEST_DATA_PACKET;
    }

    std::vector<char> construct(uint16_t requestId);
    size_t getMaxSize() const {
        return sizeof(uint16_t);
    }
};

} /* namespace network */
} /* namespace accord */

#endif
