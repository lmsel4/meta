#include <simple/simple.h>
#include <sel4/sel4.h>

#include <common.h>

//FIXME: need to redefine type because of nameclashes...
typedef uint32_t phys_addr_t;

typedef phys_addr_t resource_size_t;
typedef uint8_t u8;

// FIXME: name clash prevents from including linux headers
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

struct resource * __request_region(struct resource *res,
                                   resource_size_t start,
                                   resource_size_t n,
                                   const char *name, int flags)
{
    debug("Requesting region from %u to %u\n", start, start + n);

    struct resource* out = malloc(sizeof(struct resource));

    out->name = calloc(sizeof(char), strlen(name));
    strncpy(out->name, name, strlen(name));
    out->start = start;
    out->end = start + n;
    out->flags = flags;
    out->parent = res;

    return out;
}

void outb(unsigned char v, int port)
{
    long err = seL4_X86_IOPort_Out8(seL4_CapIOPort, port, v);
    ZF_LOGF_IFERR(err, "IOPort output failed!")
}

unsigned char inb(int port)
{
    seL4_X86_IOPort_In8_t res = seL4_X86_IOPort_In8(seL4_CapIOPort, port);

    ZF_LOGF_IFERR(res.error, "IOPort input failed!");

    return res.result;
}
