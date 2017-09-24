#include <accordcliclient/Commands.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

std::string Commands::quit(std::vector<std::string> args, int socket)
{
	close(socket);
    exit(0);
}

std::string Commands::sendMessage(std::vector<std::string> args, int socket)
{
	std::string message = "0 Hello";
	send(socket, message.c_str(), message.size(), 0);
	return "Message sent successfully";
}
