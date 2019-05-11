#include <accordserver/util/CryptoUtil.h>

#include <accordserver/log/Logger.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace accord {
namespace util {

std::vector<unsigned char> CryptoUtil::getRandomBytes(int length)
{
    std::vector<unsigned char> buffer;
    buffer.resize(length);
    int n = RAND_bytes(&buffer[0], length);
    if (n != 1) {
        log::Logger::log(log::FATAL, "Error getting random bytes!");
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

std::string CryptoUtil::sha256(const std::vector<char> &data)
{
    std::vector<unsigned char> array(SHA256_DIGEST_LENGTH);
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &data[0], data.size());
    SHA256_Final(&array[0], &ctx);
    return charToHex(std::vector<char>(array.begin(), array.end()));
}

uint64_t CryptoUtil::getRandomUINT64()
{
    uint64_t ret;
    std::vector<unsigned char> buffer = getRandomBytes(8);
    memcpy(&ret, buffer.data(), 8);
    return ret;
}

} /* namespace util */
} /* namespace accord */
