#include <accordserver/util/CryptoUtil.h>

#include <accordserver/log/Logger.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <vector>

namespace accord {
namespace util {

std::vector<unsigned char> CryptoUtil::getRandomBytes(int length)
{
    std::vector<unsigned char> buffer;
    buffer.reserve(length);
    int n = RAND_bytes(&buffer[0], length);
    if (n != 1) {
        log::Logger::log(log::ERROR, "Error getting random bytes!");
        return std::vector<unsigned char>();
    }
    return buffer;
}

std::vector<char> CryptoUtil::hexToChar(std::string hex)
{
    std::vector<char> buffer;
    buffer.resize(hex.length() / 2);
    int count = 0;
    for (unsigned int i = 0; i < (hex.length() / 2); i++) {
        std::string substr = hex.substr(count, count + 2);
        int value = strtol(substr.c_str(), NULL, 16);
        buffer[i] = (char) value;
        count += 2;
    }

    return buffer;
}

std::string CryptoUtil::charToHex(std::vector<char> array)
{
    std::vector<char> buffer;
    buffer.resize(array.size() * 2);
    int count = 0;
    for (unsigned int i = 0; i < (array.size()); i++) {
        int value = (int) array[i];
        sprintf(&buffer[count], "%02X", value);
        count += 2;
    }
    return std::string(buffer.begin(), buffer.end());
}

std::string CryptoUtil::getRandomString(int length)
{
    const std::string charIndex = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQ"
                                  "RSTUVWXYZ1234567890";
    std::vector<unsigned char> buffer = getRandomBytes(length);
    std::string ret("");
    for (int i = 0; i < length; i++) {
        std::vector<int> indexes;
        indexes.reserve(length);
        indexes[i] = ((int) buffer[i]) % charIndex.length();
        ret += charIndex[indexes[i]];
    }

    if (ret.empty())
        return getRandomString(length);
    else
        return ret;
}

uint64_t CryptoUtil::getRandomUINT64()
{
    std::vector<unsigned char> buffer = getRandomBytes(sizeof(uint64_t));
    std::string string(buffer.begin(), buffer.end());
    return strtoull(string.c_str(), NULL, 0);
}

} /* namespace util */
} /* namespace accord */
