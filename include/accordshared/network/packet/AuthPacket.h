#ifndef NETWORK_PACKET_AUTH_PACKET_H
#define NETWORK_PACKET_AUTH_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class AuthPacket : public Packet {
public:
    virtual AuthPacket *clone() const {
        return new AuthPacket();
    }

    int getId() const {
        return AUTH_PACKET;
    }

    std::vector<char> construct(const std::string &login,
                                const std::string &password);
    size_t getMaxSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
