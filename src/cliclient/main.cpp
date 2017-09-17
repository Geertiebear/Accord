#include <iostream>

#include <accordcliclient/CommandParser.h>

int main(int argc, char **argv)
{
    CommandParser parser;
    std::string input;
    while (1) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::cout << parser.parseCommand(input) << std::endl;
    }
    return 0;
}
