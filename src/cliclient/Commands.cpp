#include <accordcliclient/Commands.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <accordshared/network/packet/SendMessagePacket.h>

std::string Commands::quit(std::vector<std::string> args, int socket)
{
	close(socket);
    exit(0);
}

std::string Commands::sendMessage(std::vector<std::string> args, int socket)
{
	accord::network::SendMessagePacket packet;
	std::string message = packet.construct("Hello");;
	send(socket, message.c_str(), message.size(), 0);
	return "Message sent successfully";
}
