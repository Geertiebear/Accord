#include <iostream>

#include <accordserver/log/Logger.h>
#include <accordserver/util/LibEventUtil.h>
#include <accordserver/util/OpenSSLUtil.h>
#include <accordserver/Arguments.h>
#include <accordserver/Server.h>

int main(int argc, char **argv) {
    accord::Arguments args = accord::ArgumentParser::parse(argc, argv);
    
    accord::log::Logger::init(args.logLevel);
    accord::util::LibEventUtil::init();
    accord::util::OpenSSLUtil::opensslInit();
    accord::Server server(args);
    return 0;
}
