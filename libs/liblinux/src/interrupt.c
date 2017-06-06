#include <linux/kernel.h>
#include <linux/interrupt.h>

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags)
{
    return IRQ_HANDLED;
}
