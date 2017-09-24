#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <accordcliclient/CommandParser.h>

int main(int argc, char **argv)
{
    CommandParser parser;
    std::string input;
	int socketfd;
	struct sockaddr_in serverAddress;

	char buffer[256];
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	memset((char*) &serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(6524);

	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

	connect(socketfd, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    while (1) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::cout << parser.parseCommand(input, socketfd) << std::endl;
    }
    return 0;
}
