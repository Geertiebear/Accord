#ifndef NETWORK_PACKET_DECODER_H
#define NETWORK_PACKET_DECODER_H

#include <stddef.h>
#include <boost/ptr_container/ptr_map.hpp>

#include <accordshared/network/PacketData.h>
#include <accordshared/network/Packet.h>

namespace accord {
namespace network {

using PacketId = uint16_t;

// Helper class to decode incoming packets
class PacketDecoder {
public:
	static void init();

	static const Packet *getPacket(PacketId id);
    static int receivePacket(PacketId id, const std::vector<char> &buffer,
                             PacketData *data);
private:
	static boost::ptr_map<PacketId, Packet> packets;
};

} /* namespace network */
} /* namespace accord */

#endif
