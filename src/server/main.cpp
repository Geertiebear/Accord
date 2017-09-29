#include <iostream>

#include <accordserver/util/LibEventUtil.h>
#include <accordserver/util/OpenSSLUtil.h>
#include <accordserver/Arguments.h>
#include <accordserver/Server.h>

int main(int argc, char **argv) {
    accord::Arguments args = {
        .threads = 4,
        .port = 6524,
    };
    
    accord::util::LibEventUtil::init();
	accord::util::OpenSSLUtil::opensslInit();
    accord::Server server(args);
    return 0;
}
