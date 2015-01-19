#include <log/LogContext.hpp>

logger::LogLevel TRACE(34, "TRACE", 1, true);
logger::LogLevel DEBUG(35, "DEBUG", 5, true);

logger::LogLevel INFO("\e[01;37m", "INFO", 10);

logger::LogLevel SUCCESS(32, "SUCCESS", 10, true);

logger::LogLevel WARN(33, "WARN", 15, true);
logger::LogLevel ERROR(31, "ERROR", 20, true);
logger::LogLevel FATAL(31, "FATAL", 15, true);
