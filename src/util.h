
#ifndef UTIL_H
#define UTIL_H

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

extern void put32(ulong, uint);
extern uint get32(ulong);
extern void delay(ulong);

extern ulong getEL(void);

#endif