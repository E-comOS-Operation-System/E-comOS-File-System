#include "../include/nefs/nefs.h"
#include "../include/nefs/nefs_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("=== NEFS Test ===\n");
    
    printf("Formatting with 8KB blocks...\n");
    NEFS* fs = nefs_format("test_nefs.img", 8192);
    if (!fs) {
        printf("Failed to format\n");
        return 1;
    }
    printf("Format OK - Block size: %u\n", fs->superblock.block_size);
    printf("Total blocks: %llu, Free: %llu\n", (unsigned long long)fs->superblock.total_blocks, (unsigned long long)fs->superblock.free_blocks);
    
    printf("Creating file...\n");
    if (nefs_create_file(fs, "root>large.dat") < 0) {
        printf("Failed to create file\n");
        return 1;
    }
    
    printf("Writing 100KB data...\n");
    char* data = malloc(100 * 1024);
    memset(data, 'A', 100 * 1024);
    ssize_t written = nefs_write_file(fs, 1, data, 100 * 1024, 0);
    printf("Written %ld bytes\n", written);
    
    printf("Reading back...\n");
    char* buf = malloc(100 * 1024);
    ssize_t read = nefs_read_file(fs, 1, buf, 100 * 1024, 0);
    printf("Read %ld bytes\n", read);
    
    if (memcmp(data, buf, 100 * 1024) == 0) {
        printf("✓ Data verified!\n");
    } else {
        printf("✗ Data mismatch!\n");
    }
    
    free(data);
    free(buf);
    
    nefs_unmount(fs);
    printf("Done!\n");
    
    return 0;
}
