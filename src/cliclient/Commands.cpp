#include <accordcliclient/Commands.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <accordshared/network/packet/SendMessagePacket.h>

std::string Commands::quit(std::vector<std::string> args, SSL *ssl)
{
    SSL_free(ssl);
    exit(0);
}

std::string Commands::sendMessage(std::vector<std::string> args, SSL *ssl)
{
	accord::network::SendMessagePacket packet;
	std::vector<char> message = packet.construct("Hello");;
	SSL_write(ssl, &message[0], message.size());
	return "Message sent successfully";
}
