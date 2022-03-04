
#ifndef UTIL_H
#define UTIL_H

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

typedef uchar bool;
#define false 0
#define true 1

#define null ((void*)0)

extern void put32(ulong, uint);
extern uint get32(ulong);
extern void delay(ulong);

extern ulong getEL(void);

#define NUM_CORES 4

#endif