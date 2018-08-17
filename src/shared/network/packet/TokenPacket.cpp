#include <accordshared/network/packet/TokenPacket.h>

namespace accord {
namespace network {

std::vector<char> TokenPacket::construct(const std::string &token)
{
    std::vector<char> result;
    writeHeader(TOKEN_PACKET, token.size(), result);
    write(result, token);
    return result;
}

size_t TokenPacket::getMaxSize() const
{
    return 255;
}

} /* namespace network */
} /* namespace accord */
