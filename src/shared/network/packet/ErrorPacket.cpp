#include <accordshared/network/packet/ErrorPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

int ErrorPacket::dispatch(struct bufferevent *bufferevent, Error error)
{
	ErrorPacket packet;
    std::vector<char> result = packet.construct(error);
	return bufferevent_write(bufferevent, &result[0], result.size());
}

std::vector<char> ErrorPacket::construct(Error error)
{
	std::vector<char> result;
    result.reserve(sizeof(Error));
    write(result, ERROR_PACKET);
    write(result, (uint16_t) error);
	return result;
}

size_t ErrorPacket::getMaxSize() const
{
	return sizeof(uint16_t) + sizeof(Error);
}

} /* network */
} /* accord */
