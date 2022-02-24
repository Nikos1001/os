
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
    return 0;
}

void gpio_selectFunction(uint pin, uint alt) {
    ulong selectAddr = getSelectAddr(pin);
    uint selector = get32(selectAddr);
    uint pinShift = 3 * (pin % 10);
    selector &= ~(7 << pinShift); // clear function
    selector |= alt << pinShift; // set alternative function
    put32(selectAddr, selector);
}

void gpio_pushFunctions(uint pud, ulong pinmask) {
    uint clock = GPIO_GPPUDCLK0;
    if(pinmask >= (1 << 32)) {
        clock = GPIO_GPPUDCLK1;
        pinmask >>= 32;
    }
    put32(GPIO_GPPUD, pud);
    delay(150);
    put32(clock, pinmask);
    delay(150);
    put32(GPIO_GPPUD, 0);
    put32(clock, 0);
}

void gpio_setHighDetectEnable(uint pin, uint enable) {
    ulong addr = pin >= 32 ? GPIO_GPHEN1 : GPIO_GPHEN0;
    if(pin >= 32)
        pin -= 32;
    uint highEnable = get32(addr);
    highEnable &= ~(1 << pin);
    highEnable |= enable << pin;
    put32(addr, highEnable);
}