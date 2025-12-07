#ifndef EFS_H
#define EFS_H

#include "../common/fs_types.h"
#include "../common/block_device.h"

typedef long ssize_t;

#define EFS_MAGIC 0x45465331  // "EFS1"
#define EFS_BLOCK_SIZE 4096
#define EFS_MAX_STORAGE (256ULL * 1024 * 1024 * 1024)  // 256GiB
#define EFS_MAX_FOLDER_SIZE (20ULL * 1024 * 1024 * 1024)  // 20GiB

typedef struct {
    uint32_t magic;
    uint32_t version;
    block_t total_blocks;
    block_t free_blocks;
    inode_t total_inodes;
    inode_t free_inodes;
    block_t inode_table_block;
    block_t data_bitmap_block;
    block_t inode_bitmap_block;
    uint32_t checksum;
} efs_superblock_t;

typedef struct {
    fs_object_type_t type;
    uint64_t size;
    uint64_t created;
    uint64_t modified;
    uint32_t permissions;
    block_t direct_blocks[12];
    block_t indirect_block;
    uint32_t checksum;
} efs_inode_t;

typedef struct {
    BlockDevice* device;
    efs_superblock_t superblock;
    uint8_t* inode_bitmap;
    uint8_t* data_bitmap;
} EFS;

EFS* efs_format(const char* device_path);
EFS* efs_mount(const char* device_path);
int efs_unmount(EFS* fs);

#endif
