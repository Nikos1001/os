
#ifndef GPIO_H
#define GPIO_H

#include "../../util.h"
#include "../base.h"

#define GPIO_GPFSEL0 (PERIPHERAL_BASE + 0x00200000)
#define GPIO_GPFSEL1 (PERIPHERAL_BASE + 0x00200004)
#define GPIO_GPFSEL2 (PERIPHERAL_BASE + 0x00200008)
#define GPIO_GPFSEL3 (PERIPHERAL_BASE + 0x0020000C)
#define GPIO_GPFSEL4 (PERIPHERAL_BASE + 0x00200010)
#define GPIO_GPFSEL5 (PERIPHERAL_BASE + 0x00200014)

#define GPIO_GPPUD (PERIPHERAL_BASE + 0x00200094)
#define GPIO_GPPUDCLK0 (PERIPHERAL_BASE + 0x00200098)

void gpio_selectFunction(uint pin, uint alt);
void gpio_pushFunctions(uint pinmask);

#endif GPIO_H