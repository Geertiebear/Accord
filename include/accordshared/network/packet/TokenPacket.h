#ifndef NETWORK_PACKET_TOKEN_PACKET_H
#define NETWORK_PACKET_TOKEN_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class TokenPacket : public Packet {
public:
    virtual TokenPacket *clone() const {
        return new TokenPacket();
    }

    int getId() const {
        return TOKEN_PACKET;
    }

    std::vector<char> construct(const std::string &token);
    size_t getMaxSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
