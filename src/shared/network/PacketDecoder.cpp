#include <accordshared/network/PacketDecoder.h>

#include <accordshared/network/packet/SendMessagePacket.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/network/packet/DisconnectPacket.h>
#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/network/packet/RegisterPacket.h>
#include <accordshared/network/packet/TokenPacket.h>
#include <accordshared/network/packet/SerializationPacket.h>
#include <accordshared/util/BinUtil.h>

#include <iostream>

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
    PacketId authPacketId = AUTH_PACKET;
    packets.insert(authPacketId, new AuthPacket());
    PacketId registerPacketId = REGISTER_PACKET;
    packets.insert(registerPacketId, new RegisterPacket());
    PacketId tokenPacketId = TOKEN_PACKET;
    packets.insert(tokenPacketId, new TokenPacket());
    PacketId serializationPacketId = SERIALIZATION_PACKET;
    packets.insert(serializationPacketId, new SerializationPacket());
}

const Packet *PacketDecoder::getPacket(PacketId id)
{
	try {
		return &packets.at(id);
	} catch (boost::bad_ptr_container_operation &e) {
		return nullptr;
	}
}

int PacketDecoder::receivePacket(std::vector<char> &buffer, PacketData *data)
{
    uint8_t low = (uint8_t) buffer[0];
    uint8_t high = (uint8_t) buffer[1];

    network::PacketId packetId = util::BinUtil::assembleUint16(low, high);
    const network::Packet *packet = network::PacketDecoder::getPacket(packetId);
    if (!packet)
        return Error::NOT_FOUND;
    buffer.erase(buffer.begin(), buffer.begin() + HEADER_SIZE);
    if (buffer.size() > packet->getMaxSize()) {
        return Error::TOO_LONG;
    }
    return packet->receive(buffer, data);
}

} /* namespace network */
} /* namespace accord */
