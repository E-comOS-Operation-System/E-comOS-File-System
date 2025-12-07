#include "../../include/nefs/nefs.h"
#include "../../include/nefs/nefs_ops.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint32_t crc32(const void* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = data;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

static inode_t alloc_inode(NEFS* fs) {
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

static block_t alloc_block(NEFS* fs) {
    for (block_t i = 10000; i < fs->superblock.total_blocks; i++) {
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

static block_t get_block_at_offset(NEFS* fs, nefs_inode_t* inode, size_t offset, int alloc) {
    size_t block_idx = offset / fs->superblock.block_size;
    size_t ptrs_per_block = fs->superblock.block_size / sizeof(block_t);
    
    if (block_idx < 12) {
        if (alloc && inode->direct_blocks[block_idx] == 0) {
            inode->direct_blocks[block_idx] = alloc_block(fs);
        }
        return inode->direct_blocks[block_idx];
    }
    
    block_idx -= 12;
    if (block_idx < ptrs_per_block) {
        if (alloc && inode->indirect_block == 0) {
            inode->indirect_block = alloc_block(fs);
        }
        block_t* indirect = calloc(1, fs->superblock.block_size);
        if (inode->indirect_block != 0) {
            fs->device->read_block(fs->device, inode->indirect_block, indirect);
        }
        if (alloc && indirect[block_idx] == 0) {
            indirect[block_idx] = alloc_block(fs);
            fs->device->write_block(fs->device, inode->indirect_block, indirect);
        }
        block_t result = indirect[block_idx];
        free(indirect);
        return result;
    }
    
    block_idx -= ptrs_per_block;
    if (block_idx < ptrs_per_block * ptrs_per_block) {
        if (alloc && inode->double_indirect_block == 0) {
            inode->double_indirect_block = alloc_block(fs);
        }
        size_t l1_idx = block_idx / ptrs_per_block;
        size_t l2_idx = block_idx % ptrs_per_block;
        
        block_t* l1 = calloc(1, fs->superblock.block_size);
        if (inode->double_indirect_block != 0) {
            fs->device->read_block(fs->device, inode->double_indirect_block, l1);
        }
        if (alloc && l1[l1_idx] == 0) {
            l1[l1_idx] = alloc_block(fs);
            fs->device->write_block(fs->device, inode->double_indirect_block, l1);
        }
        
        block_t* l2 = calloc(1, fs->superblock.block_size);
        if (l1[l1_idx] != 0) {
            fs->device->read_block(fs->device, l1[l1_idx], l2);
        }
        if (alloc && l2[l2_idx] == 0) {
            l2[l2_idx] = alloc_block(fs);
            fs->device->write_block(fs->device, l1[l1_idx], l2);
        }
        block_t result = l2[l2_idx];
        free(l2);
        free(l1);
        return result;
    }
    
    return 0;
}

int nefs_create_file(NEFS* fs, const char* path) {
    if (!fs || !path) return -1;
    
    inode_t ino = alloc_inode(fs);
    if (ino == (inode_t)-1) return -1;
    
    nefs_inode_t inode = {0};
    inode.type = FS_TYPE_FILE;
    inode.size = 0;
    inode.created = inode.modified = inode.accessed = time(NULL);
    inode.permissions = 0644;
    inode.links = 1;
    inode.crc32_checksum = crc32(&inode, sizeof(nefs_inode_t) - 4);
    
    size_t inodes_per_block = fs->superblock.block_size / sizeof(nefs_inode_t);
    block_t inode_block = fs->superblock.inode_table_block + ino / inodes_per_block;
    size_t inode_offset = ino % inodes_per_block;
    
    nefs_inode_t* block_buf = calloc(1, fs->superblock.block_size);
    fs->device->read_block(fs->device, inode_block, block_buf);
    block_buf[inode_offset] = inode;
    fs->device->write_block(fs->device, inode_block, block_buf);
    free(block_buf);
    
    return 0;
}

ssize_t nefs_write_file(NEFS* fs, inode_t ino, const void* data, size_t len, size_t offset) {
    if (!fs || !data) return -1;
    
    size_t inodes_per_block = fs->superblock.block_size / sizeof(nefs_inode_t);
    block_t inode_block = fs->superblock.inode_table_block + ino / inodes_per_block;
    size_t inode_offset = ino % inodes_per_block;
    
    nefs_inode_t* block_buf = malloc(fs->superblock.block_size);
    fs->device->read_block(fs->device, inode_block, block_buf);
    nefs_inode_t inode = block_buf[inode_offset];
    
    size_t written = 0;
    while (written < len) {
        size_t block_off = (offset + written) % fs->superblock.block_size;
        size_t to_write = fs->superblock.block_size - block_off;
        if (to_write > len - written) to_write = len - written;
        
        block_t blk = get_block_at_offset(fs, &inode, offset + written, 1);
        if (blk == 0) break;
        
        uint8_t* buf = malloc(fs->superblock.block_size);
        fs->device->read_block(fs->device, blk, buf);
        memcpy(buf + block_off, (uint8_t*)data + written, to_write);
        fs->device->write_block(fs->device, blk, buf);
        free(buf);
        
        written += to_write;
    }
    
    if (offset + written > inode.size) {
        inode.size = offset + written;
    }
    inode.modified = time(NULL);
    inode.crc32_checksum = crc32(&inode, sizeof(nefs_inode_t) - 4);
    
    block_buf[inode_offset] = inode;
    fs->device->write_block(fs->device, inode_block, block_buf);
    free(block_buf);
    
    return written;
}

ssize_t nefs_read_file(NEFS* fs, inode_t ino, void* data, size_t len, size_t offset) {
    if (!fs || !data) return -1;
    
    size_t inodes_per_block = fs->superblock.block_size / sizeof(nefs_inode_t);
    block_t inode_block = fs->superblock.inode_table_block + ino / inodes_per_block;
    size_t inode_offset = ino % inodes_per_block;
    
    nefs_inode_t* block_buf = malloc(fs->superblock.block_size);
    fs->device->read_block(fs->device, inode_block, block_buf);
    nefs_inode_t inode = block_buf[inode_offset];
    
    if (offset >= inode.size) return 0;
    if (offset + len > inode.size) len = inode.size - offset;
    
    size_t read = 0;
    while (read < len) {
        size_t block_off = (offset + read) % fs->superblock.block_size;
        size_t to_read = fs->superblock.block_size - block_off;
        if (to_read > len - read) to_read = len - read;
        
        block_t blk = get_block_at_offset(fs, &inode, offset + read, 0);
        if (blk == 0) break;
        
        uint8_t* buf = malloc(fs->superblock.block_size);
        fs->device->read_block(fs->device, blk, buf);
        memcpy((uint8_t*)data + read, buf + block_off, to_read);
        free(buf);
        
        read += to_read;
    }
    
    inode.accessed = time(NULL);
    block_buf[inode_offset] = inode;
    fs->device->write_block(fs->device, inode_block, block_buf);
    free(block_buf);
    
    return read;
}
