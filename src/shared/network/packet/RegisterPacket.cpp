#include <accordshared/network/packet/RegisterPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> RegisterPacket::construct(const std::string &name,
                                            const std::string &email,
                                            const std::string &password)
{
    std::vector<char> result;
    result.reserve(HEADER_SIZE + name.length() + email.length() + password.length());
    writeHeader(&result, REGISTER_PACKET);
    std::copy(name.begin(), name.end(), std::back_inserter(result));
    result.push_back(0x3);
    std::copy(email.begin(), email.end(), std::back_inserter(result));
    result.push_back(0x3);
    std::copy(password.begin(), password.end(), std::back_inserter(result));
    return result;
}

size_t RegisterPacket::getMaxSize() const
{
    return HEADER_SIZE + (255*3);
}

} /* namespace network */
} /* namespace accord */
