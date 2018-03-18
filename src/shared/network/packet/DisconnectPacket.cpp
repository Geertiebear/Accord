#include <accordshared/network/packet/DisconnectPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> DisconnectPacket::construct() {
	std::vector<char> result;
	uint8_t low = 0, high = 0;
	util::BinUtil::splitUint16((uint16_t) DISCONNECT_PACKET, &low, &high);
	result.push_back((char)low);
	result.push_back((char)high);
	return result;
}

size_t DisconnectPacket::getBufferSize() const {
	return sizeof(uint16_t);
}

} /* network */
} /* accord */
