#include <accordshared/network/packet/RegisterPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> RegisterPacket::construct(const std::string &name,
                                            const std::string &email,
                                            const std::string &password)
{
    std::vector<char> result;
    const uint64_t length = name.size() + 1 + email.size() + 1 +
            password.size();
    writeHeader(REGISTER_PACKET, length, result);
    write(result, name);
    result.push_back(0x3);
    write(result, email);
    result.push_back(0x3);
    write(result, password);
    return result;
}

size_t RegisterPacket::getMaxSize() const
{
    return (255*3);
}

} /* namespace network */
} /* namespace accord */
