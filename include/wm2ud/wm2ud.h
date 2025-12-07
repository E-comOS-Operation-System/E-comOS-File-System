#ifndef WM2UD_H
#define WM2UD_H

#include "../common/fs_types.h"
#include "../common/block_device.h"

typedef long ssize_t;

#define WM2UD_MAGIC 0x574D3255  // "WM2U"
#define WM2UD_PAGE_SIZE 4096

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t page_size;
    uint64_t total_pages;
    uint64_t free_pages;
    uint8_t compression_enabled;
    uint8_t encryption_enabled;
    uint8_t reserved[22];
} wm2ud_superblock_t;

typedef struct {
    uint64_t virtual_addr;
    block_t physical_block;
    uint32_t compressed_size;
    uint32_t flags;
    uint64_t last_access;
} wm2ud_page_entry_t;

typedef struct {
    wm2ud_page_entry_t* entry;
    void* data;
    uint64_t last_access;
    struct wm2ud_lru_node* prev;
    struct wm2ud_lru_node* next;
} wm2ud_lru_node_t;

typedef struct {
    BlockDevice* device;
    wm2ud_superblock_t superblock;
    wm2ud_page_entry_t* page_table;
    size_t page_table_size;
    wm2ud_lru_node_t* lru_head;
    wm2ud_lru_node_t* lru_tail;
    size_t cache_size;
} WM2UD;

WM2UD* wm2ud_format(const char* device_path);
WM2UD* wm2ud_mount(const char* device_path);
int wm2ud_unmount(WM2UD* swap);

int wm2ud_write_page(WM2UD* swap, uint64_t virtual_addr, const void* data);
int wm2ud_read_page(WM2UD* swap, uint64_t virtual_addr, void* data);
int wm2ud_free_page(WM2UD* swap, uint64_t virtual_addr);

#endif
