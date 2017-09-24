#include <accordcliclient/CommandParser.h>

#include <iostream>
#include <sstream>

#include <accordcliclient/Commands.h>

CommandParser::CommandParser()
{
    commandMap.insert(std::make_pair("quit", &Commands::quit));
	commandMap.insert(std::make_pair("send", &Commands::sendMessage));
}

std::string CommandParser::parseCommand(const std::string &command, int socket)
{
    std::stringstream stringStream(command);
    std::string temp;
    std::vector<std::string> tokens;
    
    while (std::getline(stringStream, temp, ' '))
        tokens.push_back(temp);
    
    std::function<std::string(std::vector<std::string>, int)> commandFunction;
    auto it = commandMap.find(tokens[0]);
    if (it != commandMap.end()) {
        commandFunction = it->second;
        return commandFunction(tokens, socket);
    }
    
    return "Command not found!";
}

