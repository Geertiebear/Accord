#ifndef NETWORK_PACKET_DISCONNECT_PACKET_H
#define NETWORK_PACKET_DISCONNECT_PACKET_H

#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class DisconnectPacket : public  Packet {
public:
	virtual DisconnectPacket *clone() const {
		return new DisconnectPacket();
	}

	int getId() const {
		return DISCONNECT_PACKET;
	}

	std::vector<char> construct();
    size_t getMaxSize() const;
};

} /* network */
} /* accord */
#endif
