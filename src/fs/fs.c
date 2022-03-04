
#include "fs.h"

uint fs_readUint(uchar* data) {
    uint result = 0;
    for(int i = 0; i < 4; i++)
        result |= data[i] << (i * 8);
    return result;
}

uint fs_readUword(uchar* data) {
    uint result = 0;
    for(int i = 0; i < 2; i++)
        result |= data[i] << (i * 8);
    return result;
}

void fs_loadFilesystem(int core, struct fs_Filesystem* fs, uchar* mbr) {
    fs_loadPartition(&fs->partition, mbr);
    switch(fs->partition.type) {
        case FS_FAT32: {
            fs_loadFAT32(core, fs);
            break;
        }
    }
}