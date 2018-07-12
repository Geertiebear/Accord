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

} /* namespace util */
} /* namespace accord */
