
#ifndef MALLOC_H
#define MALLOC_H

#include "util.h"
#include "lock.h"

#define KERNEL_HEAP_SIZE (16 * 1024 * 1024) // 16 mb of kernel heap
extern uchar kheap[KERNEL_HEAP_SIZE];
extern uchar* currHeapPtr;

extern struct Lock mallocLock;

void initMalloc();
void* malloc(int core, ulong size);
void free(int core, void* alloc);

#endif