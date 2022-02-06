#include "dev/bus.h"
#include <dev/io.h>
#include <stdio.h>
#include <unistd.h>

uint32_t io_read(int port)
{
    switch (port)
    {
    case 0:
        return getchar();
    case 1:
        return bus_get_addr();

    case 2: // Cpu ID, 1 is caar1
        return 1;
    case 3: // running in VM
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
