#include <accordshared/network/packet/SendMessagePacket.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

//in here for consistency's sake
std::string SendMessagePacket::construct(const std::string &message)
{ 
	return std::to_string(SEND_MESSAGE_PACKET) + " " + message;
}

size_t SendMessagePacket::getBufferSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
