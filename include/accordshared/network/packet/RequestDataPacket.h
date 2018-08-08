#ifndef ACCORD_NETWORK_REQUEST_DATA_PACKET_H
#define ACCORD_NETWORK_REQUEST_DATA_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

enum RequestIds {
    COMMUNITIES_TABLE_REQUEST = 0,
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
