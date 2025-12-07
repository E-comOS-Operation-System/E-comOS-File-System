#include "../../include/efs/efs.h"
#include "../../include/efs/efs_ops.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static inode_t alloc_inode(EFS* fs) {
    for (inode_t i = 0; i < fs->superblock.total_inodes; i++) {
        size_t byte = i / 8;
        size_t bit = i % 8;
        if (!(fs->inode_bitmap[byte] & (1 << bit))) {
            fs->inode_bitmap[byte] |= (1 << bit);
            fs->superblock.free_inodes--;
            return i;
        }
    }
    return (inode_t)-1;
}

static block_t alloc_block(EFS* fs) {
    for (block_t i = 0; i < fs->superblock.total_blocks; i++) {
        size_t byte = i / 8;
        size_t bit = i % 8;
        if (!(fs->data_bitmap[byte] & (1 << bit))) {
            fs->data_bitmap[byte] |= (1 << bit);
            fs->superblock.free_blocks--;
            return i;
        }
    }
    return (block_t)-1;
}

int efs_create_file(EFS* fs, const char* path) {
    if (!fs || !path) return -1;
    if (strlen(path) > FS_MAX_PATH_LEN) return -1;
    
    inode_t ino = alloc_inode(fs);
    if (ino == (inode_t)-1) return -1;
    
    efs_inode_t inode = {0};
    inode.type = FS_TYPE_FILE;
    inode.size = 0;
    inode.created = inode.modified = time(NULL);
    inode.permissions = 0644;
    
    block_t inode_block = fs->superblock.inode_table_block + ino / (EFS_BLOCK_SIZE / sizeof(efs_inode_t));
    fs->device->write_block(fs->device, inode_block, &inode);
    
    return 0;
}

ssize_t efs_write_file(EFS* fs, inode_t ino, const void* data, size_t len, size_t offset) {
    if (!fs || !data) return -1;
    
    efs_inode_t inode;
    block_t inode_block = fs->superblock.inode_table_block + ino / (EFS_BLOCK_SIZE / sizeof(efs_inode_t));
    fs->device->read_block(fs->device, inode_block, &inode);
    
    size_t written = 0;
    while (written < len) {
        size_t block_idx = (offset + written) / EFS_BLOCK_SIZE;
        size_t block_off = (offset + written) % EFS_BLOCK_SIZE;
        size_t to_write = EFS_BLOCK_SIZE - block_off;
        if (to_write > len - written) to_write = len - written;
        
        block_t blk;
        if (block_idx < 12) {
            if (inode.direct_blocks[block_idx] == 0) {
                inode.direct_blocks[block_idx] = alloc_block(fs);
            }
            blk = inode.direct_blocks[block_idx];
        } else {
            return written;
        }
        
        uint8_t buf[EFS_BLOCK_SIZE];
        fs->device->read_block(fs->device, blk, buf);
        memcpy(buf + block_off, (uint8_t*)data + written, to_write);
        fs->device->write_block(fs->device, blk, buf);
        
        written += to_write;
    }
    
    inode.size = offset + written;
    fs->device->write_block(fs->device, inode_block, &inode);
    
    return written;
}

ssize_t efs_read_file(EFS* fs, inode_t ino, void* data, size_t len, size_t offset) {
    if (!fs || !data) return -1;
    
    efs_inode_t inode;
    block_t inode_block = fs->superblock.inode_table_block + ino / (EFS_BLOCK_SIZE / sizeof(efs_inode_t));
    fs->device->read_block(fs->device, inode_block, &inode);
    
    if (offset >= inode.size) return 0;
    if (offset + len > inode.size) len = inode.size - offset;
    
    size_t read = 0;
    while (read < len) {
        size_t block_idx = (offset + read) / EFS_BLOCK_SIZE;
        size_t block_off = (offset + read) % EFS_BLOCK_SIZE;
        size_t to_read = EFS_BLOCK_SIZE - block_off;
        if (to_read > len - read) to_read = len - read;
        
        block_t blk;
        if (block_idx < 12) {
            blk = inode.direct_blocks[block_idx];
        } else {
            return read;
        }
        
        uint8_t buf[EFS_BLOCK_SIZE];
        fs->device->read_block(fs->device, blk, buf);
        memcpy((uint8_t*)data + read, buf + block_off, to_read);
        
        read += to_read;
    }
    
    return read;
}
