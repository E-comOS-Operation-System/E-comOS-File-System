#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include "fs_types.h"

typedef struct BlockDevice {
    void* private_data;
    size_t block_size;
    block_t total_blocks;
    
    int (*read_block)(struct BlockDevice* dev, block_t block, void* buffer);
    int (*write_block)(struct BlockDevice* dev, block_t block, const void* buffer);
    int (*sync)(struct BlockDevice* dev);
} BlockDevice;

BlockDevice* block_device_open(const char* path, size_t block_size);
void block_device_close(BlockDevice* dev);

#endif
