#ifndef NETWORK_PACKET_HANDLERS_H
#define NETWORK_PACKET_HANDLERS_H

#include <string>

namespace accord {
namespace network {

class PacketHandlers {
public:
	static bool receiveSendMessagePacket(const std::string &body);
	static bool receiveErrorPacket(const std::string &body);
};

} /* namespace network */
} /* namespace accord */
#endif
