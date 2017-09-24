#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include <vector>
#include <string>

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
	virtual bool receive(const std::vector<std::string> &args) const = 0;
	virtual size_t getBufferSize() const = 0;
};

} /* namespace network */
} /* namespace accord */

#endif
