
#include "util.h"
#include "malloc.h"
#include "devices/uart/uart.h"
#include "devices/video/video.h"
#include "devices/sd/sd.h"
#include "fs/fs.h"

uchar buff[SD_SECTOR_SIZE];

struct fs_Filesystem fs;

void kernelMain(ulong core) {
    if(core == 0) {
        initMalloc();

        uart_init();

        video_init(320, 240);

        sd_init();
        sd_readSector(0, 0, buff, 1);
        fs_loadFilesystem(core, &fs, buff + 0x1be);

        uart_puts("booted\n");

        struct fs_FAT32File file;
        fs_openFAT32ClusterId(core, &fs, &file, 0xb);
        uint currC = fs_getcFAT32(core, &fs, &file);
        while(currC != FS_EOF) {
            uart_putc(currC);
            currC = fs_getcFAT32(core, &fs, &file);
        }
        fs_closeFAT32(core, &fs, &file);

    } else {
        while(1);
    }

    int off = 0;
    while(1) {
        for(int i = 0; i < gfx.h * gfx.pitch; i++)
            gfx.fb[i] = i % 2 * 255;
        video_pushFrame();
        off++;
        delay(10000000);
    }
    while(1); // hang
}