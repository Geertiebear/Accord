#include <accordshared/util/BinUtil.h>

#include <iostream>

namespace accord {
namespace util {

void BinUtil::splitUint16(uint16_t toSplit, uint8_t *low, uint8_t *high)
{
	*low = toSplit & 0xFF;
	*high = toSplit >> 8;
}

uint16_t BinUtil::assembleUint16(uint8_t low, uint8_t high)
{
	return (uint16_t) low | (uint16_t) high << 8;
}

std::vector<uint8_t> BinUtil::splitUint64(uint64_t toSplit)
{
    std::vector<uint8_t> result;
    result.push_back( (uint8_t) toSplit);
    result.push_back( (uint8_t) (toSplit >> 8));
    result.push_back( (uint8_t) (toSplit >> 16));
    result.push_back( (uint8_t) (toSplit >> 24));
    result.push_back( (uint8_t) (toSplit >> 32));
    result.push_back( (uint8_t) (toSplit >> 40));
    result.push_back( (uint8_t) (toSplit >> 48));
    result.push_back( (uint8_t) (toSplit >> 56));
    return result;
}

uint64_t BinUtil::assembleUint64(const std::vector<uint8_t> &data)
{
    return (uint64_t) data[0] | (uint64_t) data[1] << 8 |
            (uint64_t) data[2] << 16 | (uint64_t) data[3] << 24 |
            (uint64_t) data[4] << 32 | (uint64_t) data[5] << 40 |
            (uint64_t) data[6] << 48 | (uint64_t) data[7] << 56;
}

} /* namespace util */
} /* namespace accord */
