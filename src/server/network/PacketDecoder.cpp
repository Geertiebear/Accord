#include <accordserver/network/PacketDecoder.h>

#include <accordserver/network/packet/SendMessagePacket.h>

namespace accord {
namespace network {

boost::ptr_map<PacketId, Packet> PacketDecoder::packets;

void PacketDecoder::init()
{
	PacketId sendMessagePacketId = 0;
	packets.insert(sendMessagePacketId, new SendMessagePacket());
}

const Packet &PacketDecoder::getPacket(PacketId id)
{
	return packets.at(id);
}

} /* namespace network */
} /* namespace accord */
