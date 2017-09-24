#include <accordshared/network/packet/SendMessagePacket.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

//in here for consistency's sake
std::string SendMessagePacket::construct() { return ""; }

size_t SendMessagePacket::getBufferSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
