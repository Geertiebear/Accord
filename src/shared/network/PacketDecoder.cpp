#include <accordshared/network/PacketDecoder.h>

#include <accordshared/network/packet/SendMessagePacket.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/network/packet/DisconnectPacket.h>

namespace accord {
namespace network {

boost::ptr_map<PacketId, Packet> PacketDecoder::packets;

void PacketDecoder::init()
{
	PacketId sendMessagePacketId = SEND_MESSAGE_PACKET;
	packets.insert(sendMessagePacketId, new SendMessagePacket());
	PacketId errorPacketId = ERROR_PACKET;
	packets.insert(errorPacketId, new ErrorPacket());
	PacketId disconnectPacketId = DISCONNECT_PACKET;
	packets.insert(disconnectPacketId, new DisconnectPacket());
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
