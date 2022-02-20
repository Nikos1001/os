
#include "util.h"
#include "devices/mbox/mbox.h"
#include "devices/uart/uart.h"
#include "devices/video/video.h"

void kernelMain(ulong core) {
    if(core == 0) {
        uart_init();
        video_init(320, 240);
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