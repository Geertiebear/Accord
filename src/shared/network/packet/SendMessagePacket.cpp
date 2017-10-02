#include <accordshared/network/packet/SendMessagePacket.h>

#include <iostream>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> SendMessagePacket::construct(const std::string &message)
{
	std::vector<char> result;
	result.reserve(HEADER_SIZE + message.size());
	uint8_t low = 0;
	uint8_t high = 0;
	util::BinUtil::splitUint16((uint16_t) SEND_MESSAGE_PACKET, &low, &high);
	result.push_back((char) high);
	result.push_back((char) low);
	std::copy(message.begin(), message.end(), std::back_inserter(result));
	return result;
}

size_t SendMessagePacket::getBufferSize() const {
	return 2000;
}

} /* namespace network */
} /* namespace accord */
