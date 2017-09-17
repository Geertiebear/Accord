#include <accordcliclient/Commands.h>

#include <stdlib.h>

std::string Commands::quit(std::vector<std::string> args) {
    exit(0);
}
