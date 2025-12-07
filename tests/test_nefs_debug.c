#include "../include/nefs/nefs.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Step 1: Opening device\n");
    BlockDevice* dev = block_device_open("test_debug.img", 8192);
    if (!dev) {
        printf("Failed\n");
        return 1;
    }
    printf("OK: total_blocks=%llu\n", (unsigned long long)dev->total_blocks);
    
    printf("Step 2: Setting total_blocks\n");
    if (dev->total_blocks == 0) {
        dev->total_blocks = (1ULL << 30) / 8192;
    }
    printf("OK: total_blocks=%llu\n", (unsigned long long)dev->total_blocks);
    
    printf("Step 3: Allocating NEFS\n");
    NEFS* fs = malloc(sizeof(NEFS));
    printf("OK: fs=%p\n", (void*)fs);
    
    printf("Step 4: Setting superblock\n");
    fs->superblock.total_blocks = dev->total_blocks;
    fs->superblock.total_inodes = (dev->total_blocks - 10000) / 8;
    printf("OK: inodes=%llu\n", (unsigned long long)fs->superblock.total_inodes);
    
    printf("Step 5: Allocating inode_bitmap\n");
    size_t inode_bitmap_size = (fs->superblock.total_inodes + 7) / 8;
    printf("inode_bitmap_size=%zu\n", inode_bitmap_size);
    fs->inode_bitmap = calloc(1, inode_bitmap_size);
    printf("OK: inode_bitmap=%p\n", (void*)fs->inode_bitmap);
    
    printf("Step 6: Allocating data_bitmap\n");
    size_t data_bitmap_size = (fs->superblock.total_blocks + 7) / 8;
    printf("data_bitmap_size=%zu\n", data_bitmap_size);
    fs->data_bitmap = calloc(1, data_bitmap_size);
    printf("OK: data_bitmap=%p\n", (void*)fs->data_bitmap);
    
    printf("All steps passed!\n");
    
    free(fs->inode_bitmap);
    free(fs->data_bitmap);
    free(fs);
    block_device_close(dev);
    
    return 0;
}
