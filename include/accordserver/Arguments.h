#ifndef ACCORD_ARGUMENTS_H
#define ACCORD_ARGUMENTS_H

namespace accord {

struct Arguments {
    int threads;
    int port;
    int logLevel;
};

class ArgumentParser {
public:
    static Arguments parse(int argc, char **argv);
};

} /* namespace auth */

#endif
