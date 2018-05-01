#ifndef UTIL_OPEN_SSL_UTIL_H
#define UTIL_OPEN_SSL_UTIL_H

#include <vector>
#include <mutex>
#include <openssl/ssl.h>
#include <accordserver/Config.h>

namespace accord {
namespace util {

class OpenSSLUtil {
public:
	static void opensslInit();
    static SSL_CTX *getContext(const Config &config);
	static void lockCallback(int mode, int n, const char *file, int line);
	static unsigned long idCallback();
private:
	static std::mutex locks[CRYPTO_num_locks()];
};

} /* namespace util */
} /* namespace accord */

#endif
