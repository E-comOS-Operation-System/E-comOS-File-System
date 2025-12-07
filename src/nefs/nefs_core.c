#include "../../include/nefs/nefs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void sha256_stub(const void* data, size_t len, uint8_t* hash) {
    (void)data; (void)len;
    memset(hash, 0, 32);
}

NEFS* nefs_format(const char* device_path, uint32_t block_size) {
    if (block_size < NEFS_MIN_BLOCK_SIZE || block_size > NEFS_MAX_BLOCK_SIZE) {
        return NULL;
    }
    
    BlockDevice* dev = block_device_open(device_path, block_size);
    if (!dev) return NULL;
    
    if (dev->total_blocks == 0) {
        dev->total_blocks = (1ULL << 30) / block_size;
    }
    
    NEFS* fs = malloc(sizeof(NEFS));
    fs->device = dev;
    fs->current_transaction = 0;
    
    memset(&fs->superblock, 0, sizeof(nefs_superblock_t));
    fs->superblock.magic = NEFS_MAGIC;
    fs->superblock.version = 1;
    fs->superblock.block_size = block_size;
    fs->superblock.total_blocks = dev->total_blocks;
    fs->superblock.journal_blocks = 1024;
    fs->superblock.journal_start = 1;
    fs->superblock.inode_bitmap_block = 1025;
    fs->superblock.data_bitmap_block = 2025;
    fs->superblock.inode_table_block = 4025;
    
    block_t usable = dev->total_blocks > 10000 ? dev->total_blocks - 10000 : 0;
    fs->superblock.total_inodes = usable > 0 ? usable / 8 : 1024;
    fs->superblock.free_inodes = fs->superblock.total_inodes - 1;
    fs->superblock.free_blocks = usable;
    
    sha256_stub(&fs->superblock, sizeof(nefs_superblock_t) - 32, fs->superblock.sha256_checksum);
    
    size_t inode_bitmap_size = (fs->superblock.total_inodes + 7) / 8;
    fs->inode_bitmap = calloc(1, inode_bitmap_size);
    fs->inode_bitmap[0] = 1;
    
    size_t data_bitmap_size = (fs->superblock.total_blocks + 7) / 8;
    fs->data_bitmap = calloc(1, data_bitmap_size > 0 ? data_bitmap_size : 1);
    
    return fs;
}

NEFS* nefs_mount(const char* device_path) {
    BlockDevice* dev = block_device_open(device_path, NEFS_MIN_BLOCK_SIZE);
    if (!dev) return NULL;
    
    NEFS* fs = malloc(sizeof(NEFS));
    fs->device = dev;
    
    if (dev->read_block(dev, 0, &fs->superblock) < 0) {
        free(fs);
        block_device_close(dev);
        return NULL;
    }
    
    if (fs->superblock.magic != NEFS_MAGIC) {
        free(fs);
        block_device_close(dev);
        return NULL;
    }
    
    if (dev->block_size != fs->superblock.block_size) {
        block_device_close(dev);
        dev = block_device_open(device_path, fs->superblock.block_size);
        if (!dev) {
            free(fs);
            return NULL;
        }
        fs->device = dev;
    }
    
    size_t inode_bitmap_size = (fs->superblock.total_inodes + 7) / 8;
    fs->inode_bitmap = malloc(inode_bitmap_size);
    
    size_t data_bitmap_size = (fs->superblock.total_blocks + 7) / 8;
    fs->data_bitmap = malloc(data_bitmap_size);
    
    fs->current_transaction = 0;
    
    return fs;
}

int nefs_unmount(NEFS* fs) {
    if (!fs) return -1;
    
    fs->device->write_block(fs->device, 0, &fs->superblock);
    fs->device->sync(fs->device);
    
    free(fs->inode_bitmap);
    free(fs->data_bitmap);
    block_device_close(fs->device);
    free(fs);
    
    return 0;
}
