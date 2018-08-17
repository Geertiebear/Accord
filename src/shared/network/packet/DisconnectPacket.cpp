#include <accordshared/network/packet/DisconnectPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> DisconnectPacket::construct() {
	std::vector<char> result;
    writeHeader(DISCONNECT_PACKET, 0, result);
    return result;
}

size_t DisconnectPacket::getMaxSize() const {
    return 0;
}

} /* network */
} /* accord */
