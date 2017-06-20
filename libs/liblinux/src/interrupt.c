#include <sel4/sel4.h>
#include <seL4/utils.h>

#include <sel4utils/thread.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <common.h>

#define MAX_IRQ 256

typedef int (*irq_handler_t) (int, void *dev_id);

struct lmseL4_IRQHandler {
    char *name;
    irq_handler_t handler;
    unsigned int irq;
    void *dev_id;
    bool stopped;

    seL4_CPtr ep;
};


static struct lmseL4_IRQHandler* handlers[MAX_IRQ];

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn,
                         unsigned long flags, const char *name, void *dev)
{
    seL4_Error err;
    cspacepath_t irqCtrl;
    vka_object_t irq_notif = { 0 };
    struct lmseL4_IRQHandler* data = malloc(sizeof(struct lmseL4_IRQHandler));

    vka_alloc_notification(&vka, &irq_notif);
    vka_cspace_alloc_path(&vka, &irqCtrl);

    err = simple_get_IRQ_handler(&simple, 3, irqCtrl);

    ZF_LOGF_IFERR(err, "Unable to get IRQ handler");

    err = seL4_IRQHandler_SetNotification(irqCtrl.capPtr, irq_notif.cptr);

    ZF_LOGF_IFERR(err, "Unable to set notification...");

    struct lmseL4_IRQHandler tdata = {
        .name = name,
        .dev_id = dev,
        .irq = irq,
        .ep = irq_notif.cptr,
        .stopped = false,
        .handler = handler,
    };

    *data = tdata;

    handlers[irq] = data;

    return 0;
}

void free_irq(unsigned int irq, void *data)
{
    if (handlers[irq])
    {

    }
    else
    {
        ZF_LOGE("Unregisterting unregistered IRQ!");
    }
}

void interrupt_listener(struct lmseL4_IRQHandler* handler)
{
    seL4_Word badge;

    assert(handler);

    while (!handler->stopped)
    {
        seL4_Wait(handler->ep, &badge);

        printf("Got interrupt from %x", badge);

        handler->handler(handler->irq, handler->dev_id);

        seL4_IRQHandler_Ack(handler->ep);
    }

    free(handler);
}
