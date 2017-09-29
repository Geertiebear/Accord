#ifndef UTIL_OPEN_SSL_UTIL_H
#define UTIL_OPEN_SSL_UTIL_H

#include <openssl/ssl.h>

namespace accord {
namespace util {

class OpenSSLUtil {
public:
	static void opensslInit();
	static SSL_CTX *getContext();
};

} /* namespace util */
} /* namespace accord */

#endif
