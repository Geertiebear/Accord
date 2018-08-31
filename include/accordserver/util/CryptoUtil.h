#ifndef ACCORD_UTIL_CRYPTO_UTIL_H
#define ACCORD_UTIL_CRYPTO_UTIL_H

#include <vector>
#include <string>
#include <stddef.h>

namespace accord {
namespace util {

class CryptoUtil {
public:
    static std::vector<unsigned char> getRandomBytes(int length);
    static std::vector<char> hexToChar(std::string hex);
    static std::string charToHex(std::vector<char> array);
    static std::string getRandomString(int length);
    /* returns hash of data in hex form */
    static std::string sha256(const std::vector<char> &data);
    static uint64_t getRandomUINT64();
};

} /* namespace util */
} /* namespace accord */

#endif
