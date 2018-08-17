#include <accordshared/network/packet/ErrorPacket.h>

#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

std::vector<char> ErrorPacket::construct(Error error)
{
	std::vector<char> result;
    writeHeader(ERROR_PACKET, sizeof(Error), result);
    write(result, (uint16_t) error);
	return result;
}

size_t ErrorPacket::getMaxSize() const
{
    return sizeof(Error);
}

} /* network */
} /* accord */
