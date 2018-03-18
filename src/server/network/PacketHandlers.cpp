#include <accordserver/network/PacketHandlers.h>

#include <accordserver/log/Logger.h>
#include <accordserver/Server.h>
#include <accordserver/thread/Thread.h>

namespace accord {
namespace network {

bool PacketHandlers::receiveSendMessagePacket(const std::string &body, PacketData *data)
{
	thread::Client *client = (thread::Client*) data;
	client->server.broadcast(body, client->channel);
	log::Logger::log(log::DEBUG, "Received send message packet with body: " + body);
	return true;
}

bool PacketHandlers::receiveErrorPacket(const std::string &body, PacketData *data)
{
	log::Logger::log(log::DEBUG, "Received error packet with body: " + body);
	return true;
}

bool PacketHandlers::receiveDisconnectPacket(const std::string &body, PacketData *data)
{
	thread::Client *client = (thread::Client*) data;
	client->thread.removeClient(client);
	log::Logger::log(log::DEBUG, "Removed client");
	return true;
}

} /* namespace network */
} /* namespace accord */
