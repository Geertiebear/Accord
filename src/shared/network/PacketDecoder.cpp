#include <accordshared/network/PacketDecoder.h>

#include <accordshared/network/packet/SendMessagePacket.h>

namespace accord {
namespace network {

boost::ptr_map<PacketId, Packet> PacketDecoder::packets;

void PacketDecoder::init()
{
	PacketId sendMessagePacketId = SEND_MESSAGE_PACKET;
	packets.insert(sendMessagePacketId, new SendMessagePacket());
}

const Packet *PacketDecoder::getPacket(PacketId id)
{
	try {
		return &packets.at(id);
	} catch (boost::bad_ptr_container_operation &e) {
		return nullptr;
	}
}

} /* namespace network */
} /* namespace accord */
