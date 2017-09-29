#include <accordserver/util/OpenSSLUtil.h>

#include <stdexcept>

#include <accordserver/log/Logger.h>

namespace accord {
namespace util {

void OpenSSLUtil::opensslInit()
{
	SSL_load_error_strings();
	SSL_library_init();
}

SSL_CTX *OpenSSLUtil::getContext()
{
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
	if (!SSL_CTX_use_certificate_chain_file(ctx, "cert") ||
			!SSL_CTX_use_PrivateKey_file(ctx, "pkey.pem", SSL_FILETYPE_PEM)) {
		Logger::log(ERROR, "Could not find private key or certificate!");
		throw std::runtime_error("");
	}
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	return ctx;
}

} /* namespace util */
} /* namespace accord */
