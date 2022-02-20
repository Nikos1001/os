
#include "mbox.h"

volatile uint __attribute__((aligned(16))) mbox[2048];

int mbox_call(unsigned char channel) {
    uint r = (((uint)((ulong)&mbox) & ~0xF) | (channel & 0xF));
    while(get32(MBOX_STATUS) & MBOX_FULL); // wait until mailbox available
    put32(MBOX_WRITE, r);
    while(get32(MBOX_STATUS) & MBOX_EMPTY);
    return mbox[1] == MBOX_RESPONSE;
}