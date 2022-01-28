#ifndef LIB_LOG_H
#define LIB_LOG_H

typedef enum
{
    LOG_INFO,
    LOG_ERROR,
    LOG_WARN,
} LogLevel;

void _log(LogLevel level, char *fmt, ...);

#define error(msg, ...) _log(LOG_ERROR, msg __VA_OPT__(, ) __VA_ARGS__)
#define info(msg, ...) _log(LOG_INFO, msg __VA_OPT__(, ) __VA_ARGS__)
#define warn(msg, ...) _log(LOG_WARN, msg __VA_OPT__(, ) __VA_ARGS__)

#endif