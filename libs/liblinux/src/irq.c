/*#include <sel4/sel4.h>
#include <simple/simple.h>

#undef BIT
#include <linux/types.h>
#include <uapi/linux/types.h>
#include <linux/interrupt.h>

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
                const char* devname, void *dev_id)
{
    return 0;
}

void thread_handler(seL4_CPtr cap, unsigned int irq, irq_handler_t handler)
{
    seL4_Word sender;

    while(true)
    {
        seL4_Wait(cap, &sender);
        handler(irq, NULL);
    }
}
*/
