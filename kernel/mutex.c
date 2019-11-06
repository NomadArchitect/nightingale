
#include <basic.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/print.h>
#include <ng/thread.h>
#include <stdatomic.h>
#include <stdbool.h>

int print_locked = 0;

int try_acquire_mutex(kmutex *lock) {
        int unlocked = 0;
        atomic_compare_exchange_weak(lock, &unlocked, running_thread->tid + 1);
        if (!unlocked) {
                // when compare exchange fails it overwrites the expected object
                // *that*'s how you know!
                return 1;
        }
        return 0;
}

int await_mutex(kmutex *lock) {
        while (true) {
                int res = try_acquire_mutex(lock);
                if (res)
                        return res;

                if (print_locked) {
                        printf("locked:%p/%i/(%i:%i)", lock, *lock,
                               running_process->pid, running_thread->tid);
                        print_locked -= 1;
                }
                asm volatile("pause");
        }
}

int release_mutex(kmutex *lock) {
        *lock = 0;
        return 0;
}
