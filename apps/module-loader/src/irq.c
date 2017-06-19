#include <autoconf.h>

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

#include <sel4/sel4.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <vka/object.h>

#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>

#include <utils/zf_log.h>
#include <sel4utils/sel4_zf_logif.h>

#define MAX_IRQ 64

#define IRQ_INFO_VADDR 0x7000000 // arbitrary (but free) address for IRQ info

typedef int (*irq_handler_t) (int, void*);

enum irqreturn {
    IRQ_NONE		= (0 << 0),
    IRQ_HANDLED		= (1 << 0),
    IRQ_WAKE_THREAD		= (1 << 1),
};

struct irq_info {
    unsigned int irq;
    irq_handler_t handler;
    void *dev_id;
    seL4_CPtr irq_cap;
};

static struct irq_info* irq_registers[MAX_IRQ];
static unsigned int lirq = 0;

seL4_Error spawn_new_thread(seL4_CPtr irq_cap, irq_handler_t handler, unsigned int irq,
                            void *data, vka_t *vka, seL4_CPtr pd_cap)
{
    seL4_Error err;
    vka_object_t tcb = { 0 };
    vka_object_t frame_ob = { 0 };
    seL4_Word stack_top = ((seL4_Word) malloc(4096)) - 4096;
    seL4_UserContext regs = { 0 };
    size_t rsize = sizeof(seL4_UserContext) / sizeof(seL4_Word);

    err = vka_alloc_tcb(vka, &tcb);

    if (err)
        return err;

    err = vka_alloc_frame(vka, sizeof(struct irq_info), &frame_ob);

    if (err)
        return err;

    err = seL4_ARCH_Page_Map((seL4_X86_Page) frame_ob.cptr, pd_cap,
                             IRQ_INFO_VADDR, seL4_AllRights,
                             seL4_ARCH_Default_VMAttributes);

    if (err)
        return err;

    irq_registers[lirq] = malloc(sizeof(struct irq_info));
    struct irq_info* cur = irq_registers[lirq++];

    assert(cur);

    struct irq_info n = {
        .irq = irq,
        .handler = handler,
        .dev_id = data,
        .irq_cap = irq_cap,
    };

    *cur = n;

    sel4utils_set_stack_pointer(&regs, stack_top);
    err = seL4_TCB_WriteRegisters(tcb.cptr, 0, 0, rsize, &regs);

    if (err)
        return err;

    return seL4_TCB_Resume(tcb.cptr);
}

void irq_handler()
{
    // Get the irq_info for this handler from vspace
    struct irq_info* data = (struct irq_info*) IRQ_INFO_VADDR;
    seL4_Word sender;

    assert(data->handler);

    while(true)
    {
        seL4_Wait(data->irq_cap, &sender);

        while(data->handler(data->irq, data->dev_id) != IRQ_HANDLED);

        seL4_IRQHandler_Ack(data->irq_cap);
    }
}
