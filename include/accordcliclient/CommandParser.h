#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <openssl/ssl.h>
#include <string>
#include <vector>
#include <functional>
#include <map>

class CommandParser {
public:

    CommandParser();
    ~CommandParser() {}

    std::string parseCommand(const std::string &command, SSL* ssl);
private:
    std::map<std::string, std::function<std::string(
            std::vector<std::string>, SSL*)>> commandMap;
};

#endif
