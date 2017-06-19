#include <simple/simple.h>
#include <sel4/sel4.h>

#ifdef CONFIG_PHYS_ADDR_T_64BIT
typedef u64 phys_addr_t;
#else
typedef uint32_t phys_addr_t;
#endif

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

static seL4_CPtr ioport_cap;

void set_root_ioport_cap(seL4_CPtr cap)
{
    ioport_cap = cap;
}

uint8_t inb(int port)
{
    return 0;
}


uint8_t outb(uint8_t b, int port)
{
    return 0;
}
