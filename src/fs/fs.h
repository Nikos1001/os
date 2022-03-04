
#ifndef FS_H
#define FS_H

#include "../util.h"

struct fs_Partition {
    uchar status;
    uchar type;
    uint startLBA;
    uint sectors;
};

struct fs_File {
    uint fsType;
};

#define FS_EOF 256

#include "fat32/fat32.h"

#define FS_FAT32 0xc

struct fs_Filesystem {
    struct fs_Partition partition;
    union {
        struct fs_FAT32 fat32;
    } as;
};

void fs_loadPartition(struct fs_Partition* partition, uchar* mbr);
void fs_loadFilesystem(int core, struct fs_Filesystem* fs, uchar* mbr);

uint fs_readUint(uchar* data);
uint fs_readUword(uchar* data);

#endif