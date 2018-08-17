#ifndef UTIL_BIN_UTIL
#define UTIL_BIN_UTIL

#include <cstdint>
#include <vector>

namespace accord {
namespace util {

class BinUtil {
public:
	static void splitUint16(uint16_t toSplit, uint8_t *low, uint8_t *high);
	static uint16_t assembleUint16(uint8_t low, uint8_t high);
    static std::vector<uint8_t> splitUint64(uint64_t toSplit);
    static uint64_t assembleUint64(const std::vector<uint8_t> &data);
};

} /* namespace util */
} /* namespace accord */

#endif
