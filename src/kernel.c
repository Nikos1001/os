
#include "util.h"
#include "devices/uart/uart.h"
#include "devices/video/video.h"
#include "devices/sd/sd.h"

uint buff[1024 * 1024];

void kernelMain(ulong core) {
    if(core == 0) {
        uart_init();
        video_init(320, 240);
        sd_init();
        sd_readBlock(0, buff, 1);
        for(int i = 0; i < 128 / 8; i++) {
            for(int j = 0; j < 8; j++) {
                uart_puthex(buff[i * 8 + j]);
                uart_putc(' ');
            }
            uart_putc('\n');
        }
        buff[0]++;
        sd_writeBlock(buff, 0, 1);
        uart_puts("booted");

    } else {
        while(1);
    }

    int off = 0;
    while(1) {
        for(int i = 0; i < gfx.h * gfx.pitch; i++)
            gfx.fb[i] = i + off;
        video_pushFrame();
        off++;
        delay(10000000);
    }
    while(1); // hang
}