
#include "lock.h"

void initLock(struct Lock* lock) {
    for(int i = 0; i < NUM_CORES; i++)
        lock->wantsToEnter[i] = 0;
    lock->turn = 0;
}

static bool othersWant(struct Lock* lock, int core) {
    bool res = false;
    for(int i = 0; i < NUM_CORES; i++)
        if(i != core && lock->wantsToEnter[i])
            res = true;
    return res;
}

void waitLock(struct Lock* lock, int core) {
    lock->wantsToEnter[core] = true;
    while(othersWant(lock, core)) {
        if(lock->turn != core) {
            lock->wantsToEnter[core] = false;
            while(lock->turn != core);
            lock->wantsToEnter[core] = true;
        }
    }
}

void exitLock(struct Lock* lock, int core) {
    lock->turn = (lock->turn + 1) % NUM_CORES;
    lock->wantsToEnter[core] = false;
}