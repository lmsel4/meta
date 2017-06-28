#include <sel4/sel4.h>
#include <seL4/utils.h>

#include <sel4utils/irq_server.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <common.h>

#define MAX_IRQ 256

extern irq_server_t srv;

//FIXME: namespace clashes prevent from including linux headers
typedef int (*irq_handler_t) (int, void *dev_id);

struct lmseL4_IRQData {
    irq_handler_t real_handler;
    struct irq_data* irq;
    void *dev;
};

static void lmseL4_InterruptHandler(struct irq_data* data)
{
    struct lmseL4_IRQData* d = (struct lmseL4_IRQData*) data->token;

    d->real_handler(data->irq, d->dev);

    irq_data_ack_irq(data);
}

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn,
                         unsigned long flags, const char *name, void *dev)
{
    debug("Trying to register IRQ %d", irq);

    struct lmseL4_IRQData *token = malloc(sizeof(struct lmseL4_IRQData));
    ZF_LOGF_IF(token == NULL, "Unable to request memory");

    struct irq_data* data = irq_server_register_irq(srv, irq, lmseL4_InterruptHandler,
                                                    token);
    ZF_LOGF_IF(data == NULL, "Unable to request irq from irq_server");

    token->real_handler = handler;
    token->irq = data;
    token->dev = dev;

    debug("Registered IRQ %d", irq);

    return 0;
}

void free_irq(unsigned int irq, void *data)
{
    // FIXME: it seems the irq_server does not allow freeing irq...
    debug("Tried to free IRQ %d", irq);
}
