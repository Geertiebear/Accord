#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> AuthPacket::construct(const std::string &login,
                                        const std::string &password)
{
    std::vector<char> result;
    result.reserve(HEADER_SIZE + login.length() + password.length());
    uint8_t low = 0, high = 0;
    util::BinUtil::splitUint16((uint16_t) AUTH_PACKET, &low, &high);
    result.push_back((char)low);
    result.push_back((char)high);
    std::copy(login.begin(), login.end(), std::back_inserter(result));
    result.push_back(0x3);
    std::copy(password.begin(), password.end(), std::back_inserter(result));
    return result;
}

size_t AuthPacket::getMaxSize() const
{
    return 255*2;
}

} /* namespace nework */
} /* namespace accord */
