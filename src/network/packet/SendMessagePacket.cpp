#include <network/packet/SendMessagePacket.h>

#include <log/Logger.h>

namespace accord {
namespace network {

//in here for consistency's sake
std::string SendMessagePacket::construct() { return ""; }

bool SendMessagePacket::receive(const std::vector<std::string> &args) const {
	Logger::log(DEBUG, "SendMessagePacket: received with args[0]: " + args[0]);
	return true;
}

size_t SendMessagePacket::getBufferSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
