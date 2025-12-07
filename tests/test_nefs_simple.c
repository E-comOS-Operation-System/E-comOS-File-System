#include "../include/common/block_device.h"
#include <stdio.h>

int main() {
    printf("Testing block device...\n");
    BlockDevice* dev = block_device_open("test_simple.img", 8192);
    if (!dev) {
        printf("Failed to open device\n");
        return 1;
    }
    printf("Device opened: block_size=%zu, total_blocks=%llu\n", 
           dev->block_size, (unsigned long long)dev->total_blocks);
    
    char buf[8192] = "Hello";
    printf("Writing block 0...\n");
    if (dev->write_block(dev, 0, buf) < 0) {
        printf("Write failed\n");
        return 1;
    }
    printf("Write OK\n");
    
    block_device_close(dev);
    printf("Done\n");
    return 0;
}
