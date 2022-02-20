
#include "uart.h"
#include "../gpio/gpio.h"
#include "../../util.h"

void uart_putc(char c) {
    while(1) { // wait until minuart is ready to send
        if(get32(UART_MU_LSR_REG) & 0x20)
            break;
    }
    put32(UART_MU_IO_REG, c);
}

void uart_puts(const char* s) {
    while(*s != '\0')
        uart_putc(*s++);
}

void uart_putd(int d) {
    if(d < 0) {
        uart_putc('-');
        d = -d;
    }
    if(d == 0) {
        uart_putc('0');
        return;
    }
    char buf[10];
    int curr = 0;
    while(d > 0) {
        buf[curr] = '0' + d % 10;
        d /= 10;
        curr++;
    }
    for(int i = curr - 1; i >= 0; i--) {
        uart_putc(buf[i]);
    }
}

void uart_init(void) {
    gpio_selectFunction(14, 1);
    gpio_selectFunction(15, 1);
    gpio_pushFunctions((1 << 14) | (1 << 15));

    put32(UART_ENABLE, 1); // enable miniuart
    put32(UART_MU_CNTL_REG, 0); // disable receive and transmit
    put32(UART_MU_IER_REG, 0); // disable interrupts
    put32(UART_MU_LCR_REG, 3); // 8-bit mode
    put32(UART_MU_MCR_REG, 0); // set rts to high
    put32(UART_MU_CNTL_REG, 3); // enable receive and transmit
}