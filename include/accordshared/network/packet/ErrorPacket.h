#ifndef NETWORK_PACKET_ERROR_PACKET_H
#define NETWORK_PACKET_ERROR_PACKET_H

#include <accordshared/error/ErrorCodes.h>
#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

class ErrorPacket : public Packet {
public:
	virtual ErrorPacket *clone() const {
		return new ErrorPacket();
	}

	std::string construct(Error error);
	bool receive(const std::vector<std::string> &args) const;
	size_t getBufferSize() const;
};

} /* namespace network */
} /* namespace accord */

#endif
