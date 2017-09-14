#include <iostream>

#include <util/LibEventUtil.h>
#include <Arguments.h>
#include <Server.h>

int main(int argc, char **argv) {
    accord::Arguments args = {
        .threads = 4,
        .port = 6524,
    };
    
	 accord::util::LibEventUtil::init();
    accord::Server server(args);
    return 0;
}
