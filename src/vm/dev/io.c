#include <dev/io.h>
#include <stdio.h>

uint8_t io_read(int port)
{
    switch (port)
    {
    case 1:
        warn("TODO: bus");
        break;
    case 2: // Cpu ID, 1 is caar1
        return 1;

    default:
        break;
    }

    return 0;
}

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
