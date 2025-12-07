#include "../../include/efs/efs.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint32_t crc32(const void* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = data;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

EFS* efs_format(const char* device_path) {
    BlockDevice* dev = block_device_open(device_path, EFS_BLOCK_SIZE);
    if (!dev) return NULL;
    
    block_t total_blocks = EFS_MAX_STORAGE / EFS_BLOCK_SIZE;
    inode_t total_inodes = total_blocks / 4;
    
    EFS* fs = malloc(sizeof(EFS));
    fs->device = dev;
    
    memset(&fs->superblock, 0, sizeof(efs_superblock_t));
    fs->superblock.magic = EFS_MAGIC;
    fs->superblock.version = 1;
    fs->superblock.total_blocks = total_blocks;
    fs->superblock.free_blocks = total_blocks - 100;
    fs->superblock.total_inodes = total_inodes;
    fs->superblock.free_inodes = total_inodes - 1;
    fs->superblock.inode_bitmap_block = 1;
    fs->superblock.data_bitmap_block = 10;
    fs->superblock.inode_table_block = 20;
    fs->superblock.checksum = crc32(&fs->superblock, sizeof(efs_superblock_t) - 4);
    
    dev->write_block(dev, 0, &fs->superblock);
    
    size_t inode_bitmap_size = (total_inodes + 7) / 8;
    fs->inode_bitmap = calloc(1, inode_bitmap_size);
    fs->inode_bitmap[0] = 1;  // Root inode
    
    size_t data_bitmap_size = (total_blocks + 7) / 8;
    fs->data_bitmap = calloc(1, data_bitmap_size);
    
    return fs;
}

EFS* efs_mount(const char* device_path) {
    BlockDevice* dev = block_device_open(device_path, EFS_BLOCK_SIZE);
    if (!dev) return NULL;
    
    EFS* fs = malloc(sizeof(EFS));
    fs->device = dev;
    
    if (dev->read_block(dev, 0, &fs->superblock) < 0) {
        free(fs);
        block_device_close(dev);
        return NULL;
    }
    
    if (fs->superblock.magic != EFS_MAGIC) {
        free(fs);
        block_device_close(dev);
        return NULL;
    }
    
    size_t inode_bitmap_size = (fs->superblock.total_inodes + 7) / 8;
    fs->inode_bitmap = malloc(inode_bitmap_size);
    
    size_t data_bitmap_size = (fs->superblock.total_blocks + 7) / 8;
    fs->data_bitmap = malloc(data_bitmap_size);
    
    return fs;
}

int efs_unmount(EFS* fs) {
    if (!fs) return -1;
    
    fs->device->write_block(fs->device, 0, &fs->superblock);
    fs->device->sync(fs->device);
    
    free(fs->inode_bitmap);
    free(fs->data_bitmap);
    block_device_close(fs->device);
    free(fs);
    
    return 0;
}
