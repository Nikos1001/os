
#include "util.h"

// https://en.wikipedia.org/wiki/Dekker%27s_algorithm

struct Lock {
    bool wantsToEnter[NUM_CORES];
    int turn;
};

void initLock(struct Lock* lock);
void waitLock(struct Lock* lock, int core);
void exitLock(struct Lock* lock, int core);