#include <lib/log.h>
#include <stdarg.h>
#include <stdio.h>

void _log(LogLevel level, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    switch (level)
    {
    case LOG_ERROR:
    {
        printf("\033[1;31merror\033[0m ");
        break;
    }

    case LOG_INFO:
    {
        printf("\033[1;32minfo\033[0m ");
        break;
    }

    case LOG_WARN:
    {
        printf("\033[1;33mwarn\033[0m ");
        break;
    }
    }

    vprintf(fmt, ap);

    puts("");

    va_end(ap);
}