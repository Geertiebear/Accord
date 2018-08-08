#ifndef UTIL_BIN_UTIL
#define UTIL_BIN_UTIL

#include <cstdint>

namespace accord {
namespace util {

class BinUtil {
public:
	static void splitUint16(uint16_t toSplit, uint8_t *low, uint8_t *high);
	static uint16_t assembleUint16(uint8_t low, uint8_t high);
};

} /* namespace util */
} /* namespace accord */

#endif
