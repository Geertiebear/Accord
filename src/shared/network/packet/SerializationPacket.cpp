#include <accordshared/network/packet/SerializationPacket.h>

namespace accord {
namespace network {

std::vector<char> SerializationPacket::construct(const std::string &data)
{
    std::vector<char> result;
    write(result, SERIALIZATION_PACKET);
    write(result, data);
    return result;
}

size_t SerializationPacket::getMaxSize() const
{
    return -1; //no max size maybe fix later?
}

} /* namespace network */
} /* namespace accord */
