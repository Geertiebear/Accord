#include <accordcliclient/Commands.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <accordshared/network/packet/SendMessagePacket.h>
#include <accordshared/network/packet/DisconnectPacket.h>
#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/network/packet/RegisterPacket.h>

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
    SSL_read(ssl, &buffer, sizeof(buffer));
    return std::string(buffer);
}

std::string Commands::auth(std::vector<std::string> args, SSL *ssl)
{
    accord::network::AuthPacket packet;
    std::vector<char> message = packet.construct(args[0], args[1]);
    SSL_write(ssl, &message[0], message.size());
    return "Auth successful!"; // for now
}

std::string Commands::regist(std::vector<std::string> args, SSL *ssl)
{
    accord::network::RegisterPacket packet;
    std::vector<char> message = packet.construct(args[0], args[1], args[2]);
    SSL_write(ssl, &message[0], message.size());
    return "Register successful!"; // for now
}
