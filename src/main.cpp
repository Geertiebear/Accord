#include <iostream>

#include <Arguments.h>
#include <Server.h>

int main(int argc, char **argv) {
    accord::Arguments args = {
        .threads = 4,
        .port = 6524,
    };
    
    accord::Server server(args);
    return 0;
}
