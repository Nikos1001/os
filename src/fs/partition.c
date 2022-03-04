
#include "fs.h"
#include "../devices/uart/uart.h"

// https://en.m.wikipedia.org/wiki/Master_boot_record#PTE
void fs_loadPartition(struct fs_Partition* partition, uchar* data) {
    partition->status = data[0];
    partition->type = data[4];
    partition->startLBA = fs_readUint(data + 0x8);
    partition->sectors = fs_readUint(data + 0xc);
}

