#ifndef ACCORD_PACKET_SERIALIZATION_PACKET_H
#define ACCORD_PACKET_SERIALIZATION_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class SerializationPacket : public Packet {
public:
    virtual SerializationPacket *clone() const {
        return new SerializationPacket();
    }

    int getId() const {
        return SERIALIZATION_PACKET;
    }

    std::vector<char> construct(uint16_t id, const std::vector<char> &data);
    size_t getMaxSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
