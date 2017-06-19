#include <stdio.h>
#include <stdlib.h>

#undef NULL
#include <linux/ioport.h>

struct resource ioport_resource;

uint8_t inb(int port)
{
    return 0;
}


uint8_t outb(uint8_t b, int port)
{
    return 0;
}
