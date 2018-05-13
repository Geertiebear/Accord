#ifndef COMMANDS_H
#define COMMANDS_H

#include <openssl/ssl.h>
#include <vector>
#include <string>

class Commands {
public:
    static std::string quit(std::vector<std::string>, SSL *ssl);
    static std::string sendMessage(std::vector<std::string>, SSL *ssl);
	static std::string recv(std::vector<std::string>, SSL *ssl);
    static std::string auth(std::vector<std::string>, SSL *ssl);
};

#endif
