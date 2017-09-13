#include <log/Logger.h>

#include <iostream>

namespace accord {
    
void Logger::log(LogLevel level, std::string message)
{
    std::string levelString = logLeveLString(level);
    std::cout << levelString << ": " << message << std::endl;
}

std::string Logger::logLeveLString(accord::LogLevel level)
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
    
} /* namespace accord */
