#include <accordserver/util/OpenSSLUtil.h>

#include <thread>
#include <stdexcept>

#include <accordserver/log/Logger.h>

namespace accord {
namespace util {

std::mutex OpenSSLUtil::locks[CRYPTO_num_locks()];

void OpenSSLUtil::opensslInit()
{
	SSL_load_error_strings();
	SSL_library_init();
	CRYPTO_set_id_callback(&OpenSSLUtil::idCallback);
	CRYPTO_set_locking_callback(&OpenSSLUtil::lockCallback);
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

void OpenSSLUtil::lockCallback(int mode, int n, const char *file, int line)
{
	if(mode & CRYPTO_LOCK)
		locks[n].lock();
	else
		locks[n].unlock();
}

unsigned long OpenSSLUtil::idCallback()
{
	return (unsigned long) std::hash<std::thread::id>()(std::this_thread::get_id());
}

} /* namespace util */
} /* namespace accord */
