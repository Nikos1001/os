
#ifndef MBOX_H
#define MBOX_H

#include "../base.h"
#include "../../util.h"

#define MBOX_BASE (PERIPHERAL_BASE + 0xB880)
#define MBOX_READ (MBOX_BASE + 0x0)
#define MBOX_POLL (MBOX_BASE + 0x10)
#define MBOX_SENDER (MBOX_BASE + 0x14)
#define MBOX_STATUS (MBOX_BASE + 0x18)
#define MBOX_CONFIG (MBOX_BASE + 0x1C)
#define MBOX_WRITE (MBOX_BASE + 0x20)

#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

#define MBOX_REQUEST 0

// Channels
#define MBOX_CH_POWER 0
#define MBOX_CH_FB 1
#define MBOX_CH_VUART 2
#define MBOX_CH_VCHIQ 3
#define MBOX_CH_LEDS 4
#define MBOX_CH_BTNS 5
#define MBOX_CH_TOUCH 6
#define MBOX_CH_COUNT 7
#define MBOX_CH_PROP 8

// Tags
#define MBOX_TAG_SETPOWER 0x28001
#define MBOX_TAG_SETCLKRATE 0x38002
#define MBOX_TAG_LAST 0

extern volatile uint mbox[2048];

int mbox_call(uchar channel);

#endif