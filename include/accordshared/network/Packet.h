#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include <string>

#include <accordshared/network/PacketHandler.h>

namespace accord {
namespace network {

enum PacketIds {
	SEND_MESSAGE_PACKET = 0,
	ERROR_PACKET = 1,
};

class Packet {
public:
	virtual ~Packet() { }; //base class
	virtual Packet *clone() const = 0; //polymorphic construction
	virtual size_t getBufferSize() const = 0;
	virtual int getId() const {
		return -1;
	}

	bool receive(std::string body) const
	{
		return PacketHandler::handle(getId(), body);
	}
};

} /* namespace network */
} /* namespace accord */

#endif
