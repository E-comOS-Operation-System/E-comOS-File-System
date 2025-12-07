#include "../include/wm2ud/wm2ud.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("=== WM2UD Test ===\n");
    
    printf("Formatting swap partition...\n");
    WM2UD* swap = wm2ud_format("test_swap.img");
    if (!swap) {
        printf("Failed to format\n");
        return 1;
    }
    printf("Format OK - Page size: %u\n", swap->superblock.page_size);
    printf("Total pages: %llu, Free: %llu\n", 
           (unsigned long long)swap->superblock.total_pages,
           (unsigned long long)swap->superblock.free_pages);
    
    printf("\nWriting pages...\n");
    char page1[4096], page2[4096];
    memset(page1, 'A', 4096);
    memset(page2, 'B', 4096);
    
    wm2ud_write_page(swap, 0x1000, page1);
    wm2ud_write_page(swap, 0x2000, page2);
    printf("Written 2 pages\n");
    
    printf("\nReading pages...\n");
    char buf1[4096], buf2[4096];
    wm2ud_read_page(swap, 0x1000, buf1);
    wm2ud_read_page(swap, 0x2000, buf2);
    
    if (memcmp(buf1, page1, 4096) == 0 && memcmp(buf2, page2, 4096) == 0) {
        printf("✓ Data verified!\n");
    } else {
        printf("✗ Data mismatch!\n");
    }
    
    printf("\nFreeing page...\n");
    wm2ud_free_page(swap, 0x1000);
    printf("Free pages: %llu\n", (unsigned long long)swap->superblock.free_pages);
    
    wm2ud_unmount(swap);
    printf("\nDone!\n");
    
    return 0;
}
