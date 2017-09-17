#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <functional>
#include <map>

class CommandParser {
public:

    CommandParser();
    ~CommandParser() {}

    std::string parseCommand(const std::string &command);
private:
    std::map<std::string, std::function<std::string(
            std::vector<std::string>)>> commandMap;
};

#endif
