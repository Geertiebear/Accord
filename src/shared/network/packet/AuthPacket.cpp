#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> AuthPacket::construct(const std::string &login,
                                        const std::string &password)
{
    std::vector<char> result;
    result.reserve(HEADER_SIZE + login.length() + password.length());
    write(result, AUTH_PACKET);
    write(result, login);
    result.push_back(0x3);
    write(result, password);
    return result;
}

size_t AuthPacket::getMaxSize() const
{
    return HEADER_SIZE + (255*2);
}

} /* namespace nework */
} /* namespace accord */
