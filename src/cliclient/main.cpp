#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#ifndef _WIN32
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
#else
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0600
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#include <stdlib.h>
#include <openssl/ssl.h>

#include <accordcliclient/CommandParser.h>

SSL_CTX *initCTX()
{
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_method());
    return ctx;
}

int main(int argc, char **argv)
{
    CommandParser parser;
    std::string input;
    int socketfd;
#ifndef _WIN32
    struct sockaddr_in serverAddress;
#else
	SOCKADDR_IN serverAddress;
#endif
    SSL_CTX *ctx = initCTX();
    
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char*) &serverAddress, '\0', sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6524);
#ifndef _WIN32
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
#else
	InetPtonA(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
#endif

    connect(socketfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socketfd);
    SSL_connect(ssl);
    while (1) {
        std::cout << "> ";
        std::getline(std::cin, input);
		if (input.size())
			std::cout << parser.parseCommand(input, ssl) << std::endl;
    }
    return 0;
}
