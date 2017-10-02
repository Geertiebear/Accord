#include <accordshared/network/packet/ErrorPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

int ErrorPacket::dispatch(struct bufferevent *bufferevent, Error error)
{
	ErrorPacket packet;
	std::vector<char> result;
	std::vector<char> constructed = packet.construct(error);
	result.reserve(constructed.size() + HEADER_SIZE);
	uint8_t low = 0;
	uint8_t high = 0;
	util::BinUtil::splitUint16((uint16_t) ERROR_PACKET, &low, &high);
	result.push_back(high);
	result.push_back(low);
	std::copy(constructed.begin(), constructed.end(), std::back_inserter(result));

	return bufferevent_write(bufferevent, &result[0], result.size());
}

std::vector<char> ErrorPacket::construct(Error error)
{
	std::vector<char> result;
	result.reserve(sizeof(Error));
	uint8_t low = 0;
	uint8_t high = 0;
	util::BinUtil::splitUint16((uint16_t) error, &low, &high);
	return result;
}

size_t ErrorPacket::getBufferSize() const
{
	return sizeof(uint16_t) + sizeof(Error);
}

} /* network */
} /* accord */
