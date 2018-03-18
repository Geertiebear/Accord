#ifndef NETWORK_PACKET_HANDLERS_H
#define NETWORK_PACKET_HANDLERS_H

#include <string>

#include <accordshared/network/PacketData.h>

namespace accord {
namespace network {

class PacketHandlers {
public:
	static bool receiveSendMessagePacket(const std::string &body, PacketData *data);
	static bool receiveErrorPacket(const std::string &body, PacketData *data);
	static bool receiveDisconnectPacket(const std::string &body, PacketData *data);
};

} /* namespace network */
} /* namespace accord */
#endif
