#include <linux/wait.h>
#include <linux/sched.h>

int remove_wake_func(wait_queue_t *wait, unsigned mode, int flags, void *key)
{
    return 0;
}

__always_inline void __wake_up(wait_queue_head_t *q, unsigned int mode, int nr,
                               void *key)
{
    spin_unlock(&q->lock);
}

void init_wait_entry(wait_queue_t* q, int flags)
{
    // TODO: anything to do here?
}

__always_inline void __init_waitqueue_head(wait_queue_head_t *q,
                                           const char* name,
                                           struct lock_class_key *key)
{
    spin_lock_init(&q->lock);
    INIT_LIST_HEAD(&q->task_list);
    spin_lock(&q->lock);
}

__always_inline long prepare_to_wait_event(wait_queue_head_t *q,
                                           wait_queue_t *wait, int state)
{
    spin_lock(&q->lock);
}

__always_inline void finish_wait(wait_queue_head_t *q, wait_queue_t *wait)
{
    spin_unlock(&q->lock);
}

void schedule()
{
    // HACK: this does nothing since we don't have access to the scheduler
}

int _cond_resched()
{
    return 0;
}
