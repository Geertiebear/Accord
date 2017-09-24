#include <accordshared/network/PacketHandler.h>

namespace accord {
namespace network {

std::vector<ReceiveHandler> PacketHandler::lookupTable;

void PacketHandler::init(const std::vector<ReceiveHandler> &handlers)
{
	lookupTable = handlers;
}	

bool PacketHandler::handle(int packetId, std::string body)
{
	ReceiveHandler handler = lookupTable[packetId];
	return handler(body);
}

} /* namespace network */
} /* namespace accord */
