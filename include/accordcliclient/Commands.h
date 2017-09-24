#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

class Commands {
public:
    static std::string quit(std::vector<std::string>, int socket);
	static std::string sendMessage(std::vector<std::string>, int socket);
};

#endif
