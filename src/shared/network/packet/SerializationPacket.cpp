#include <accordshared/network/packet/SerializationPacket.h>

namespace accord {
namespace network {

std::vector<char> SerializationPacket::construct(const std::string &data)
{
    std::vector<char> result;
    writeHeader(&result, SERIALIZATION_PACKET);
    std::copy(data.begin(), data.end(), std::back_inserter(result));
    return result;
}

size_t SerializationPacket::getMaxSize() const
{
    return -1; //no max size maybe fix later?
}

} /* namespace network */
} /* namespace accord */
