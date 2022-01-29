#include <lib/log.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _log(LogLevel level, char *module, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    module = strdup(module);

    module[strlen(module) - 2] = '\0';

    switch (level)
    {
    case LOG_ERROR:
    {
        printf("\033[1;31merror\033[0m ");
        break;
    }

    case LOG_INFO:
    {
        printf("\033[1;32minfo\033[0m  ");
        break;
    }

    case LOG_WARN:
    {
        printf("\033[1;33mwarn\033[0m  ");
        break;
    }
    }

    printf("\033[1;36m[%s]\033[0m ", module);

    vprintf(fmt, ap);

    puts("");

    va_end(ap);
}