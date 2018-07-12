#include <accordshared/network/packet/TokenPacket.h>

namespace accord {
namespace network {

std::vector<char> TokenPacket::construct(const std::string &token)
{
    std::vector<char> result;
    result.reserve(HEADER_SIZE + token.length());
    writeHeader(&result, TOKEN_PACKET);
    std::copy(token.begin(), token.end(), std::back_inserter(result));
    return result;
}

size_t TokenPacket::getMaxSize() const
{
    return 255 + HEADER_SIZE;
}

} /* namespace network */
} /* namespace accord */
