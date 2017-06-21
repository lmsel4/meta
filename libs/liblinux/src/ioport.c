#include <simple/simple.h>
#include <sel4/sel4.h>

#include <common.h>

#ifdef CONFIG_PHYS_ADDR_T_64BIT
typedef u64 phys_addr_t;
#else
typedef uint32_t phys_addr_t;
#endif

seL4_CPtr io_cap;

typedef phys_addr_t resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct resource *parent, *sibling, *child;
};

struct resource ioport_resource = {

};

int request_region(int port, int nr, UNUSED const char* name)
{
    printf("Request IOPort region from %d to %d\n", port, port + nr);

    io_cap = simple_get_IOPort_cap(simple, port, port + nr);

    return 0;
}

uint8_t inb(int port)
{
    return 0;
}


uint8_t outb(uint8_t b, int port)
{
    return 0;
}
