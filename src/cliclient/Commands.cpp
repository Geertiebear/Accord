#include <accordcliclient/Commands.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <accordshared/network/packet/SendMessagePacket.h>
#include <accordshared/network/packet/DisconnectPacket.h>

std::string Commands::quit(std::vector<std::string> args, SSL *ssl)
{
	accord::network::DisconnectPacket packet;
	std::vector<char> message = packet.construct();
	SSL_write(ssl, &message[0], message.size());
    SSL_free(ssl);
    exit(0);
}

std::string Commands::sendMessage(std::vector<std::string> args, SSL *ssl)
{
	accord::network::SendMessagePacket packet;
	std::string msg;
	for (size_t i = 0; i < args.size(); i++) {
		std::string token = args[i];
		if (i)
			msg += " ";
		msg += token;
	}
	std::vector<char> message = packet.construct(msg);
	if (SSL_write(ssl, &message[0], message.size()) <= 0)
      return "Error when sending message!";
    else
      return "Message sent successfully";
}

std::string Commands::recv(std::vector<std::string> args, SSL *ssl)
{
		char buffer[256];
        int n;
        ioctl(SSL_get_fd(ssl), FIONREAD, &n);
        if (!n)
			return "No message to be read!";
		SSL_read(ssl, &buffer, sizeof(buffer));
        return std::string(buffer);
}
