#include <accordshared/network/Packet.h>
#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

void Packet::writeHeader(std::vector<char> *msg, int id)
{
    uint8_t low = 0, high = 0;
    util::BinUtil::splitUint16((uint16_t) id, &low, &high);
    msg->push_back((char)low);
    msg->push_back((char)high);
}

} /* namsepace network */
} /* namespace accord */
