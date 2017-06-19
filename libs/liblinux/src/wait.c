#include <linux/wait.h>
#include <linux/sched.h>

__always_inline void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr, void *key)
{
    // TODO: implement waking threads from wait_queue
}

void init_wait_entry(wait_queue_t* q, int flags)
{
    // TODO: implement wait_entry init
}

__always_inline void __init_wait_queue_head(wait_queue_head_t *q)
{

}

__always_inline long prepare_to_wait_event(wait_queue_head_t *q, wait_queue_t *wait, int state)
{

}

__always_inline void finish_wait(wait_queue_head_t *q, wait_queue_t *wait)
{

}

void schedule()
{
    // HACK: this does nothing since we don't have access to the scheduler
}

int _cond_resched()
{
    return 0;
}
