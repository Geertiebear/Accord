#include <accordshared/network/packet/RequestDataPacket.h>

namespace accord {
namespace network {

std::vector<char> RequestDataPacket::construct(uint16_t requestId)
{
    std::vector<char> res;
    writeHeader(REQUEST_DATA_PACKET, sizeof(uint16_t), res);
    write(res, requestId);
    return res;
}

} /* namespace network */
} /* namespace accord */
