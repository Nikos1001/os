
#include "gpio.h"

static ulong getSelectAddr(uint pin) {
    switch(pin / 10) {
        case 0:
            return GPIO_GPFSEL0;
        case 1:
            return GPIO_GPFSEL1;
        case 2:
            return GPIO_GPFSEL2;
        case 3:
            return GPIO_GPFSEL3;
        case 4:
            return GPIO_GPFSEL4;
        case 5:
            return GPIO_GPFSEL5;
    }
}

void gpio_selectFunction(uint pin, uint alt) {
    ulong selectAddr = getSelectAddr(pin);
    uint selector = get32(selectAddr);
    uint pinShift = 3 * (pin % 10);
    selector &= ~(7 << pinShift); // clear function
    selector |= alt << pinShift; // set alternative function
    put32(selectAddr, selector);
}

void gpio_pushFunctions(uint pinmask) {
    put32(GPIO_GPPUD, 0);
    delay(150);
    put32(GPIO_GPPUDCLK0, pinmask);
    delay(150);
    put32(GPIO_GPPUDCLK0, 0);
}