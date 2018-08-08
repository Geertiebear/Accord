#include <accordshared/network/packet/DisconnectPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> DisconnectPacket::construct() {
	std::vector<char> result;
    write(result, DISCONNECT_PACKET);
	return result;
}

size_t DisconnectPacket::getMaxSize() const {
    return HEADER_SIZE;
}

} /* network */
} /* accord */
