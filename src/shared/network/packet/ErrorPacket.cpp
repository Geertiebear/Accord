#include <accordshared/network/packet/ErrorPacket.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

int ErrorPacket::dispatch(struct bufferevent *bufferevent, Error error)
{
	ErrorPacket packet;
	std::string message = std::to_string(ERROR_PACKET) + " " + packet.construct(error);
	return bufferevent_write(bufferevent, message.c_str(), message.size());
}

std::string ErrorPacket::construct(Error error)
{ 
	return std::to_string(static_cast<int>(error)); 
}

size_t ErrorPacket::getBufferSize() const
{
	return sizeof(uint16_t);
}

} /* network */
} /* accord */
