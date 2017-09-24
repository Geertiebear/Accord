#include <accordshared/network/packet/SendMessagePacket.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

//in here for consistency's sake
std::string SendMessagePacket::construct() { return ""; }

bool SendMessagePacket::receive(std::string body) const {
	Logger::log(DEBUG, "SendMessagePacket: received with body: " + body);
	return true;
}

size_t SendMessagePacket::getBufferSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
