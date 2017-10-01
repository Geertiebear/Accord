#include <accordserver/log/Logger.h>

#include <iostream>

namespace accord {
namespace log {

bool Logger::isInit = false;
int Logger::level;

void Logger::init(int plevel)
{
    if (isInit)
        return;
    level = plevel;
    isInit = true;
}

void Logger::log(LogLevel plevel, const std::string &message)
{
    if (plevel <= level) {
        std::string levelString = logLeveLString(plevel);
        std::cout << levelString << ": " << message << std::endl;
    }
}

std::string Logger::logLeveLString(LogLevel level)
{
    switch(level) {
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARNING";
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        default:
            return "";
    }
}

} /* namespace log */
} /* namespace accord */
