#include "../../include/wm2ud/wm2ud.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

WM2UD* wm2ud_format(const char* device_path) {
    BlockDevice* dev = block_device_open(device_path, WM2UD_PAGE_SIZE);
    if (!dev) return NULL;
    
    if (dev->total_blocks == 0) {
        dev->total_blocks = (1ULL << 30) / WM2UD_PAGE_SIZE;
    }
    
    WM2UD* swap = malloc(sizeof(WM2UD));
    swap->device = dev;
    
    memset(&swap->superblock, 0, sizeof(wm2ud_superblock_t));
    swap->superblock.magic = WM2UD_MAGIC;
    swap->superblock.version = 1;
    swap->superblock.page_size = WM2UD_PAGE_SIZE;
    swap->superblock.total_pages = dev->total_blocks - 1;
    swap->superblock.free_pages = swap->superblock.total_pages;
    swap->superblock.compression_enabled = 0;
    swap->superblock.encryption_enabled = 0;
    
    swap->page_table_size = 1024;
    swap->page_table = calloc(swap->page_table_size, sizeof(wm2ud_page_entry_t));
    swap->lru_head = NULL;
    swap->lru_tail = NULL;
    swap->cache_size = 0;
    
    return swap;
}

WM2UD* wm2ud_mount(const char* device_path) {
    BlockDevice* dev = block_device_open(device_path, WM2UD_PAGE_SIZE);
    if (!dev) return NULL;
    
    WM2UD* swap = malloc(sizeof(WM2UD));
    swap->device = dev;
    
    if (dev->read_block(dev, 0, &swap->superblock) < 0) {
        free(swap);
        block_device_close(dev);
        return NULL;
    }
    
    if (swap->superblock.magic != WM2UD_MAGIC) {
        free(swap);
        block_device_close(dev);
        return NULL;
    }
    
    swap->page_table_size = 1024;
    swap->page_table = calloc(swap->page_table_size, sizeof(wm2ud_page_entry_t));
    swap->lru_head = NULL;
    swap->lru_tail = NULL;
    swap->cache_size = 0;
    
    return swap;
}

int wm2ud_unmount(WM2UD* swap) {
    if (!swap) return -1;
    
    swap->device->sync(swap->device);
    
    free(swap->page_table);
    block_device_close(swap->device);
    free(swap);
    
    return 0;
}
