#ifndef NEFS_H
#define NEFS_H

#include "../common/fs_types.h"
#include "../common/block_device.h"

typedef long ssize_t;

#define NEFS_MAGIC 0x4E454653  // "NEFS"
#define NEFS_MIN_BLOCK_SIZE 4096
#define NEFS_MAX_BLOCK_SIZE 65536

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t block_size;
    block_t total_blocks;
    block_t free_blocks;
    inode_t total_inodes;
    inode_t free_inodes;
    block_t journal_start;
    block_t journal_blocks;
    block_t inode_table_block;
    block_t data_bitmap_block;
    block_t inode_bitmap_block;
    uint8_t sha256_checksum[32];
} nefs_superblock_t;

typedef struct {
    fs_object_type_t type;
    uint64_t size;
    uint64_t created;
    uint64_t modified;
    uint64_t accessed;
    uint32_t permissions;
    uint32_t links;
    block_t direct_blocks[12];
    block_t indirect_block;
    block_t double_indirect_block;
    block_t triple_indirect_block;
    uint32_t crc32_checksum;
} nefs_inode_t;

typedef struct {
    uint32_t transaction_id;
    uint64_t timestamp;
    uint32_t type;
    block_t target_block;
    uint32_t data_len;
    uint8_t data[4000];
} nefs_journal_entry_t;

typedef struct {
    BlockDevice* device;
    nefs_superblock_t superblock;
    uint8_t* inode_bitmap;
    uint8_t* data_bitmap;
    uint32_t current_transaction;
} NEFS;

NEFS* nefs_format(const char* device_path, uint32_t block_size);
NEFS* nefs_mount(const char* device_path);
int nefs_unmount(NEFS* fs);

#endif
