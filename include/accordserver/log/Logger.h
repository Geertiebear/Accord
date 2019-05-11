#ifndef ACCORD_LOG_LOGGER_H
#define ACCORD_LOG_LOGGER_H

#include <string>

namespace accord {
namespace log {

enum LogLevel {
    FATAL = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
};
    
class Logger {
public:
    static void init(int plevel);
    static void log(LogLevel plevel,const std::string &message);
private:
    static std::string logLeveLString(LogLevel level);

    static bool isInit;
    static int level;
};

} /* namespace log */
} /* namespace auth */

#endif
