#ifndef NETWORK_PACKET_ERROR_PACKET_H
#define NETWORK_PACKET_ERROR_PACKET_H

#include <event2/bufferevent.h>

#include <vector>

#include <accordshared/error/ErrorCodes.h>
#include <accordshared/network/Packet.h>

namespace accord {

namespace thread {

struct Client;

} /* namespace thread */

namespace network {

class ErrorPacket : public Packet {
public:
	virtual ErrorPacket *clone() const {
		return new ErrorPacket();
	}

	int getId() const {
		return ERROR_PACKET;
	}
	
	std::vector<char> construct(Error error);
    size_t getMaxSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
