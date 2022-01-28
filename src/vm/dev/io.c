#include <dev/io.h>
#include <stdio.h>

void io_write(int port, uint8_t data)
{
    switch (port)
    {
    case 0:
        putchar(data);
        break;
    case 1:
        warn("Cannot write to bus");
        break;
    }
}