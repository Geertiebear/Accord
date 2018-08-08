#ifndef NETWORK_PACKET_HANDLERS_H
#define NETWORK_PACKET_HANDLERS_H

#include <vector>

#include <accordshared/network/PacketData.h>

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
    static bool receiveRequestDataPacket(const std::vector<char> &body, PacketData *data);
};

} /* namespace network */
} /* namespace accord */
#endif
