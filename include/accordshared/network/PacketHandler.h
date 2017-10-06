#ifndef NETWORK_PACKET_HANDLER_H
#define NETWORK_PACKET_HANDLER_H

#include <vector>
#include <functional>

#include <accordshared/network/PacketData.h>

namespace accord {
namespace network {

using ReceiveHandler = std::function<bool(const std::string&, PacketData*)>;

class PacketHandler {
public:
	static void init(const std::vector<ReceiveHandler> &handlers);
	static bool handle(int packetId, std::string body, PacketData *data);
private:
	static std::vector<ReceiveHandler> lookupTable;
};

} /* namespace network */
} /* namespace accord */

#endif
