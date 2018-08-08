#include <accordshared/network/packet/RequestDataPacket.h>

namespace accord {
namespace network {

std::vector<char> RequestDataPacket::construct(uint16_t requestId)
{
    std::vector<char> res;
    write(res, REQUEST_DATA_PACKET);
    write(res, requestId);
    return res;
}

} /* namespace network */
} /* namespace accord */
