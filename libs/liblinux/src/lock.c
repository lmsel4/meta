#include <linux/spinlock.h>
#include <linux/atomic.h>

int _raw_spin_trylock(raw_spinlock_t *lock)
{
    struct qspinlock *qlock = &(lock)->raw_lock;

    if (!atomic_read(&qlock->val) &&
        (atomic_cmpxchg_acquire(&qlock->val, 0, _Q_LOCKED_VAL) == 0))
        return 1;
    return 0;
}
