#ifndef NETWORK_PACKET_REGISTER_PACKET_H
#define NETWORK_PACKET_REGISTER_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class RegisterPacket : public Packet {
public:
    virtual RegisterPacket *clone() const {
        return new RegisterPacket();
    }

    int getId() const {
        return REGISTER_PACKET;
    }

    std::vector<char> construct(const std::string &name,
                                const std::string &email,
                                const std::string &password);
    size_t getMaxSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
