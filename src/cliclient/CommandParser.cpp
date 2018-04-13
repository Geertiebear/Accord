#include <accordcliclient/CommandParser.h>

#include <iostream>
#include <sstream>

#include <accordcliclient/Commands.h>

CommandParser::CommandParser()
{
    commandMap.insert(std::make_pair("quit", &Commands::quit));
    commandMap.insert(std::make_pair("send", &Commands::sendMessage));
	commandMap.insert(std::make_pair("recv", &Commands::recv));
}

std::string CommandParser::parseCommand(const std::string &command, SSL *ssl)
{
    std::stringstream stringStream(command);
    std::string temp;
    std::vector<std::string> tokens;
    
    while (std::getline(stringStream, temp, ' '))
        tokens.push_back(temp);
    
    std::function<std::string(std::vector<std::string>, SSL*)> commandFunction;
    auto it = commandMap.find(tokens[0]);
    if (it != commandMap.end()) {
        commandFunction = it->second;
		std::vector<std::string> args;
		std::copy(tokens.begin() + 1, tokens.end(), std::back_inserter(args));
        return commandFunction(args, ssl);
    }
    
    return "Command not found!";
}

