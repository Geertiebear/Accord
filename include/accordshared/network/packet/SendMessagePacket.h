#ifndef NETWORK_PACKET_SEND_MESSAGE_PACKET_H
#define NETWORK_PACKET_SEND_MESSAGE_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class SendMessagePacket : public Packet {
public:
	virtual SendMessagePacket *clone() const {
		return new SendMessagePacket();
	}

	std::string construct();
	bool receive(std::string body) const;
	size_t getBufferSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
