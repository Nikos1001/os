
#include "malloc.h"

uchar kheap[KERNEL_HEAP_SIZE];
uchar* currHeapPtr;

struct Lock mallocLock;

void initMalloc() {
    currHeapPtr = kheap;
    initLock(&mallocLock);
}

void* malloc(int core, ulong size) {
    waitLock(&mallocLock, core);
    currHeapPtr += size;
    void* result = currHeapPtr - size;
    exitLock(&mallocLock, core);
    return result; // good enough for now
}

void free(int core, void* alloc) {
    waitLock(&mallocLock, core);
    // wow look n o t h i n g
    exitLock(&mallocLock, core);
}