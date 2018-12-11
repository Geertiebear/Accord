#ifndef NETWORK_PACKET_HANDLERS_H
#define NETWORK_PACKET_HANDLERS_H

#include <vector>

#include <accordshared/network/PacketData.h>
#include <accordshared/util/Serialization.h>

namespace accord {
namespace network {

class PacketHandlers {
public:
    static bool receiveSendMessagePacket(const std::vector<char> &body, PacketData *data);
    static bool receiveErrorPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveDisconnectPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveAuthPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveRegisterPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveNoopPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveSerializationPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveKeepAlivePacket(const std::vector<char> &body, PacketData *data);

    static util::FunctionMap serializationMap;
    static bool handleCommunitiesTable(PacketData *data, const std::vector<char> &body);
    static bool handleAddCommunityRequest(PacketData *data, const std::vector<char> &body);
    static bool handleChannels(PacketData *data, const std::vector<char> &body);
    static bool handleTokenAuth(PacketData *data, const std::vector<char> &body);
    static bool handleMessagesRequest(PacketData *data, const std::vector<char> &body);
    static bool handleSubmitMessage(PacketData *data, const std::vector<char> &body);
    static bool handleAddChannel(PacketData *data, const std::vector<char> &body);
    static bool handleUser(PacketData *data, const std::vector<char> &body);
    static bool handleSendInvite(PacketData *data, const std::vector<char> &body);
    static bool handleGenInvite(PacketData *data, const std::vector<char> &body);
};

} /* namespace network */
} /* namespace accord */
#endif
