#ifndef AUTH_ARGUMENTS_H
#define AUTH_ARGUMENTS_H

namespace accord {

struct Arguments {
    int threads;
    int port;
};

class ArgumentParser {
public:
    static Arguments parse(int argc, char **argv);
};

} /* namespace auth */

#endif
