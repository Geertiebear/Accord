#ifndef AUTH_LOG_LOGGER_H
#define AUTH_LOG_LOGGER_H

#include <string>

namespace accord {
    
enum LogLevel {
    ERROR,
    WARNING,
    INFO,
    DEBUG,
};
    
class Logger {
public:
    static void log(LogLevel level, std::string message);
private:
    static std::string logLeveLString(LogLevel level);
};

} /* namespace auth */

#endif
