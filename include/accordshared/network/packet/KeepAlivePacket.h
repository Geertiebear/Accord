#ifndef NETWORK_PACKET_KEEP_ALIVE_PACKET
#define NETWORK_PACKET_KEEP_ALIVE_PACKET

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class KeepAlivePacket : public Packet {
public:
    virtual KeepAlivePacket *clone() const {
        return new KeepAlivePacket();
    }

    int getId() const {
        return KEEP_ALIVE_PACKET;
    }

    std::vector<char> construct() {
        std::vector<char> result;
        writeHeader(KEEP_ALIVE_PACKET, 0, result);
        return result;
    }

    size_t getMaxSize() const { return 0; }
};

} /* namespace network */
} /* namespace accord */

#endif
