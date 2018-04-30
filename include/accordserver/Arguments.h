#ifndef ACCORD_ARGUMENTS_H
#define ACCORD_ARGUMENTS_H

#include <string>

namespace accord {

struct Arguments {
    int threads;
    int port;
    int logLevel;
    std::string config;
};

class ArgumentParser {
public:
    static Arguments parse(int argc, char **argv);
};

} /* namespace auth */

#endif
