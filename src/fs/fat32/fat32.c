
#include "fat32.h"
#include "../../devices/sd/sd.h"
#include "../../devices/uart/uart.h"
#include "../../malloc.h"

void fs_loadFAT32(int core, struct fs_Filesystem* fs) {
    uchar buf[SD_SECTOR_SIZE];

    // parse bios parameter block
    sd_readSector(core, fs->partition.startLBA, buf, 1); // TODO: add error checking
    fs->as.fat32.sectorsPerCluster = buf[0x0d];
    fs->as.fat32.reservedSectors = fs_readUword(buf + 0x0e);
    fs->as.fat32.sectorsPerFAT = fs_readUint(buf + 0x24);

    // get cluster end
    sd_readSector(core, fs->partition.startLBA + fs->as.fat32.reservedSectors, buf, 1);
    fs->as.fat32.clusterEnd = fs_readUint(buf + 0x04);

    // read fat
    fs->as.fat32.fat = malloc(core, fs->as.fat32.sectorsPerFAT * SD_SECTOR_SIZE);
    if(fs->as.fat32.fat == null)
        return;
    for(int i = 0; i < fs->as.fat32.sectorsPerFAT; i++) {
        sd_readSector(core, fs->partition.startLBA + fs->as.fat32.reservedSectors + i, buf, 1);
        for(int j = 0; j < SD_SECTOR_SIZE / sizeof(uint); j++)
            fs->as.fat32.fat[i * (SD_SECTOR_SIZE / sizeof(uint)) + j] = fs_readUint(buf + sizeof(uint) * j);
    }
}

static uint clusterIdToLBA(struct fs_Filesystem* fs, uint cluster) {
    return fs->partition.startLBA +
           fs->as.fat32.reservedSectors +
           2 * fs->as.fat32.sectorsPerFAT +
           (cluster - 2) * fs->as.fat32.sectorsPerCluster;
}

static void readCluster(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file) {
    uint firstLBA = clusterIdToLBA(fs, file->clusterId);
    for(int i = 0; i < fs->as.fat32.sectorsPerCluster; i++)
        sd_readSector(core, firstLBA + i, file->cluster + i * SD_SECTOR_SIZE, 1);
}

void fs_openFAT32ClusterId(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file, uint clusterId) {
    file->clusterId = clusterId;
    file->cluster = malloc(core, fs->as.fat32.sectorsPerCluster * SD_SECTOR_SIZE);
    readCluster(core, fs, file);
    file->readIdx = 0;
}

void fs_closeFAT32(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file) {
    free(core, file->cluster);
}


uint fs_getcFAT32(int core, struct fs_Filesystem* fs, struct fs_FAT32File* file) {
    if(file->clusterId == fs->as.fat32.clusterEnd) // TODO: get file size from dir info
        return FS_EOF;
    uint result = file->cluster[file->readIdx];
    file->readIdx++;
    if(file->readIdx == fs->as.fat32.sectorsPerCluster * SD_SECTOR_SIZE) {
        file->readIdx = 0;
        file->clusterId = fs->as.fat32.fat[file->clusterId];
        if(file->clusterId != fs->as.fat32.clusterEnd) {
            readCluster(core, fs, file);
        }
    }
    return result;
}