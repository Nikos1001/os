
#ifndef FAT32_H
#define FAT32_H

#include "../../util.h"

struct fs_FAT32 {
    uint sectorsPerCluster;
    uint sectorsPerFAT;
    uint reservedSectors;
    uint clusterEnd;
    uint* fat;
};

#include "../fs.h"

struct fs_FAT32File {
    struct fs_File f;
    uint clusterId;
    uchar* cluster;
    uint readIdx;
};

void fs_loadFAT32(int core, struct fs_Filesystem* fs);
void fs_openFAT32ClusterId(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file, uint clusterId);
void fs_closeFAT32(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file);

uint fs_getcFAT32(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file);

#endif