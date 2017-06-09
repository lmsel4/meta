#include <linux/kernel.h>
#include <linux/interrupt.h>

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                     irq_handler_t thread_fn,
                     unsigned long flags, const char *name, void *dev)
{
    return IRQ_HANDLED;
}
