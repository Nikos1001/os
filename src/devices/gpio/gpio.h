
#ifndef GPIO_H
#define GPIO_H

#include "../../util.h"
#include "../base.h"

#define GPIO_GPFSEL0 (PERIPHERAL_BASE + 0x00200000)
#define GPIO_GPFSEL1 (PERIPHERAL_BASE + 0x00200004)
#define GPIO_GPFSEL2 (PERIPHERAL_BASE + 0x00200008)
#define GPIO_GPFSEL3 (PERIPHERAL_BASE + 0x0020000c)
#define GPIO_GPFSEL4 (PERIPHERAL_BASE + 0x00200010)
#define GPIO_GPFSEL5 (PERIPHERAL_BASE + 0x00200014)

#define GPIO_GPPUD (PERIPHERAL_BASE + 0x00200094)
#define GPIO_GPPUDCLK0 (PERIPHERAL_BASE + 0x00200098)
#define GPIO_GPPUDCLK1 (PERIPHERAL_BASE + 0x0020009c)

#define GPIO_GPHEN0 (PERIPHERAL_BASE + 0x00200064)
#define GPIO_GPHEN1 (PERIPHERAL_BASE + 0x00200068)

void gpio_selectFunction(uint pin, uint alt);
void gpio_pushFunctions(uint pud, ulong pinmask);
void gpio_setHighDetectEnable(uint pin, uint enable);

#endif