#include <accordshared/network/packet/ErrorPacket.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

int ErrorPacket::dispatch(struct bufferevent *bufferevent, Error error)
{
	Logger::log(DEBUG, "Called ErrorPacket::dispatch()");
	ErrorPacket packet;
	std::string message = std::to_string(ERROR_PACKET) + " " + packet.construct(error);
	return bufferevent_write(bufferevent, message.c_str(), message.size());
}

std::string ErrorPacket::construct(Error error)
{ 
	return std::to_string(static_cast<int>(error)); 
}

bool ErrorPacket::receive(const std::vector<std::string> &args) const
{
	Logger::log(DEBUG, "ErrorPacket: received with args[0]: " + args[0]);
	return true;
}

size_t ErrorPacket::getBufferSize() const
{
	return sizeof(uint16_t);
}

} /* network */
} /* accord */
