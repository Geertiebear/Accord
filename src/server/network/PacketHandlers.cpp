#include <accordserver/network/PacketHandlers.h>

#include <accordserver/log/Logger.h>

namespace accord {
namespace network {

bool PacketHandlers::receiveSendMessagePacket(const std::string &body)
{
	log::Logger::log(log::DEBUG, "Received send message packet with body: " + body);
	return true;
}

bool PacketHandlers::receiveErrorPacket(const std::string &body)
{
	log::Logger::log(log::DEBUG, "Received error packet with body: " + body);
	return true;
}

} /* namespace network */
} /* namespace accord */
