#include <iostream>
#include <Magick++.h>
#include <mysql.h>

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
    if (mysql_library_init(0, nullptr, nullptr)) {
        accord::log::Logger::log(accord::log::ERROR, "Error init"
                                                     "ializing libmysqlclient");
        exit(1);

        if (!mysql_thread_safe()) {
            accord::log::Logger::log(accord::log::ERROR, "mysql library"
                                                         "is not thread-safe!");
            exit(1);
        }
    }

    Magick::InitializeMagick(argv[0]);
    accord::Server server(args);
    mysql_library_end();
    return 0;
}
