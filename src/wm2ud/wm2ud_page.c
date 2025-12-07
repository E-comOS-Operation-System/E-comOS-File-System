#include "../../include/wm2ud/wm2ud.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static size_t hash_addr(uint64_t addr, size_t table_size) {
    return (addr / WM2UD_PAGE_SIZE) % table_size;
}

static block_t alloc_block(WM2UD* swap) {
    for (block_t i = 1; i < swap->device->total_blocks; i++) {
        int found = 0;
        for (size_t j = 0; j < swap->page_table_size; j++) {
            if (swap->page_table[j].physical_block == i) {
                found = 1;
                break;
            }
        }
        if (!found) {
            swap->superblock.free_pages--;
            return i;
        }
    }
    return 0;
}

static void lru_touch(WM2UD* swap, size_t idx) {
    swap->page_table[idx].last_access = time(NULL);
}

static size_t lru_evict(WM2UD* swap) {
    uint64_t oldest = (uint64_t)-1;
    size_t victim = 0;
    
    for (size_t i = 0; i < swap->page_table_size; i++) {
        if (swap->page_table[i].physical_block != 0 && 
            swap->page_table[i].last_access < oldest) {
            oldest = swap->page_table[i].last_access;
            victim = i;
        }
    }
    
    return victim;
}

int wm2ud_write_page(WM2UD* swap, uint64_t virtual_addr, const void* data) {
    if (!swap || !data) return -1;
    
    size_t idx = hash_addr(virtual_addr, swap->page_table_size);
    
    while (swap->page_table[idx].physical_block != 0 && 
           swap->page_table[idx].virtual_addr != virtual_addr) {
        idx = (idx + 1) % swap->page_table_size;
    }
    
    if (swap->page_table[idx].physical_block == 0) {
        block_t blk = alloc_block(swap);
        if (blk == 0) {
            size_t victim = lru_evict(swap);
            blk = swap->page_table[victim].physical_block;
            swap->page_table[victim].physical_block = 0;
        }
        swap->page_table[idx].physical_block = blk;
        swap->page_table[idx].virtual_addr = virtual_addr;
    }
    
    swap->device->write_block(swap->device, swap->page_table[idx].physical_block, data);
    lru_touch(swap, idx);
    
    return 0;
}

int wm2ud_read_page(WM2UD* swap, uint64_t virtual_addr, void* data) {
    if (!swap || !data) return -1;
    
    size_t idx = hash_addr(virtual_addr, swap->page_table_size);
    
    while (swap->page_table[idx].physical_block != 0) {
        if (swap->page_table[idx].virtual_addr == virtual_addr) {
            swap->device->read_block(swap->device, swap->page_table[idx].physical_block, data);
            lru_touch(swap, idx);
            return 0;
        }
        idx = (idx + 1) % swap->page_table_size;
    }
    
    return -1;
}

int wm2ud_free_page(WM2UD* swap, uint64_t virtual_addr) {
    if (!swap) return -1;
    
    size_t idx = hash_addr(virtual_addr, swap->page_table_size);
    
    while (swap->page_table[idx].physical_block != 0) {
        if (swap->page_table[idx].virtual_addr == virtual_addr) {
            swap->page_table[idx].physical_block = 0;
            swap->page_table[idx].virtual_addr = 0;
            swap->superblock.free_pages++;
            return 0;
        }
        idx = (idx + 1) % swap->page_table_size;
    }
    
    return -1;
}
