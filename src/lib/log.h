#ifndef LIB_LOG_H
#define LIB_LOG_H

typedef enum
{
    LOG_INFO,
    LOG_ERROR,
    LOG_WARN,
} LogLevel;

void _log(LogLevel level, char *module, char *fmt, ...);
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define error(msg, ...) _log(LOG_ERROR, __FILENAME__, msg __VA_OPT__(, ) __VA_ARGS__)
#define info(msg, ...) _log(LOG_INFO, __FILENAME__, msg __VA_OPT__(, ) __VA_ARGS__)
#define warn(msg, ...) _log(LOG_WARN, __FILENAME__, msg __VA_OPT__(, ) __VA_ARGS__)
// HACK: lol
extern char *strdup(const char *);

#endif