#ifndef CAAR_IO_H
#define CAAR_IO_H
#include <lib/log.h>
#include <stdint.h>

void io_write(int port, uint8_t data);
uint8_t io_read(int port);

#endif
