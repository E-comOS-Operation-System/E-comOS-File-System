#include "../include/efs/efs.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        return 1;
    }
    
    printf("Formatting %s with EFS...\n", argv[1]);
    
    EFS* fs = efs_format(argv[1]);
    if (!fs) {
        fprintf(stderr, "Failed to format device\n");
        return 1;
    }
    
    printf("EFS formatted successfully\n");
    printf("Total blocks: %lu\n", fs->superblock.total_blocks);
    printf("Total inodes: %lu\n", fs->superblock.total_inodes);
    
    efs_unmount(fs);
    return 0;
}
