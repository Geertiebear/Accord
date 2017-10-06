#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include <string>

#include <accordshared/network/PacketHandler.h>
#include <accordshared/network/PacketData.h>

namespace accord {
namespace network {

#define HEADER_SIZE 2

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

	bool receive(std::string body, PacketData *data) const
	{
		return PacketHandler::handle(getId(), body, data);
	}
};

} /* namespace network */
} /* namespace accord */

#endif
