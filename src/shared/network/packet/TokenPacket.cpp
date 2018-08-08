#include <accordshared/network/packet/TokenPacket.h>

namespace accord {
namespace network {

std::vector<char> TokenPacket::construct(const std::string &token)
{
    std::vector<char> result;
    result.reserve(HEADER_SIZE + token.length());
    write(result, TOKEN_PACKET);
    write(result, token);
    return result;
}

size_t TokenPacket::getMaxSize() const
{
    return 255 + HEADER_SIZE;
}

} /* namespace network */
} /* namespace accord */
