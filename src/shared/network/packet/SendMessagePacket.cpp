#include <accordshared/network/packet/SendMessagePacket.h>

#include <iostream>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> SendMessagePacket::construct(const std::string &message)
{
	std::vector<char> result;
	result.reserve(HEADER_SIZE + message.size());
    writeHeader(&result, SEND_MESSAGE_PACKET);
	std::copy(message.begin(), message.end(), std::back_inserter(result));
	return result;
}

size_t SendMessagePacket::getMaxSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
