#include <sel4/sel4.h>
#include <seL4/utils.h>

#include <sel4utils/irq_server.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <common.h>

#define MAX_IRQ 256

extern irq_server_t srv;

typedef int (*irq_handler_t) (int, void *dev_id);

struct lmseL4_IRQData {
    irq_handler_t real_handler;
    struct irq_data* irq;
    void *dev;
};


/**
 * Mapper from irq_server callback to the actual module provided callback
 * @param irq the irq number to handle
 * @param dev the pointer to lmsel4_IRQData we passed when registering the irq
 **/
static void lmseL4_InterruptHandler(irq_t irq, void *dev)
{
    struct lmseL4_IRQData *data = (struct lmseL4_IRQData *) dev;

    data->real_handler(irq, data->dev);

    irq_data_ack_irq(data->irq);
}

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn,
                         unsigned long flags, const char *name, void *dev)
{
    struct lmseL4_IRQData *token = malloc(sizeof(struct lmseL4_IRQData));
    struct irq_data* data = irq_server_register_irq(srv, irq, handler,
                                                    lmseL4_InterruptHandler);

    assert(data);
    assert(token);

    ZF_LOGF_IF(data == NULL, "Unable to request irq from irq_server");

    token->real_handler = handler;
    token->irq = data;
    token->dev = dev;

    return 0;
}

void free_irq(unsigned int irq, void *data)
{
    // FIXME: it seems the irq_server does not allow freeing irq...
}
