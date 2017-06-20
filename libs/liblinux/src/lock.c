#include <linux/spinlock.h>
#include <linux/atomic.h>
#include <asm-generic/qspinlock_types.h>

int _raw_spin_trylock(raw_spinlock_t *lock)
{
    struct qspinlock *qlock = &(lock)->raw_lock;

    if (!atomic_read(&qlock->val) &&
        (atomic_cmpxchg_acquire(&qlock->val, 0, _Q_LOCKED_VAL) == 0))
        return 1;
    return 0;
}


void _raw_spin_lock(raw_spinlock_t *lock)
{
    __raw_spin_lock(lock);
}

void queued_spin_lock_slowpath(struct qspinlock *q, u32 val)
{
    while (atomic_cmpxchg(&q->val, val, _Q_LOCKED_VAL) != val);
}
