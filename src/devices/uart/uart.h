
#ifndef UART_H
#define UART_H

#include "../base.h"
#include "../../util.h"

#define UART_ENABLE (PERIPHERAL_BASE + 0x00215004)
#define UART_MU_IO_REG (PERIPHERAL_BASE + 0x00215040)
#define UART_MU_IER_REG (PERIPHERAL_BASE + 0x00215044)
#define UART_MU_IIR_REG (PERIPHERAL_BASE + 0x00215048)
#define UART_MU_LCR_REG (PERIPHERAL_BASE + 0x0021504C)
#define UART_MU_MCR_REG (PERIPHERAL_BASE + 0x00215050)
#define UART_MU_LSR_REG (PERIPHERAL_BASE + 0x00215054)
#define UART_MU_MSR_REG (PERIPHERAL_BASE + 0x00215058)
#define UART_MU_SCRATCH (PERIPHERAL_BASE + 0x0021505C)
#define UART_MU_CNTL_REG (PERIPHERAL_BASE + 0x00215060)
#define UART_MU_STAT_REG (PERIPHERAL_BASE + 0x00215064)
#define UART_MU_BAUD_REG (PERIPHERAL_BASE + 0x00215068)

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char* s);
void uart_putd(int d);
void uart_puthex(uint d);

#endif