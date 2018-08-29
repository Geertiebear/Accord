#include <accordshared/network/packet/SerializationPacket.h>

namespace accord {
namespace network {

std::vector<char> SerializationPacket::construct(uint16_t id,
                                                 const std::vector<char> &data)
{
    std::vector<char> result;
    writeHeader(SERIALIZATION_PACKET, sizeof(uint16_t) + data.size(), result);
    write(result, id);
    write(result, data);
    return result;
}

size_t SerializationPacket::getMaxSize() const
{
    return -1; //no max size maybe fix later?
}

} /* namespace network */
} /* namespace accord */
