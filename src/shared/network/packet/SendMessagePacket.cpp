#include <accordshared/network/packet/SendMessagePacket.h>

#include <iostream>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> SendMessagePacket::construct(const std::string &message)
{
	std::vector<char> result;
    writeHeader(SEND_MESSAGE_PACKET, message.length(), result);
    write(result, message);
	return result;
}

size_t SendMessagePacket::getMaxSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
