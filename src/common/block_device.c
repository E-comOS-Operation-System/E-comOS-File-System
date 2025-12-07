#include "../../include/common/block_device.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
    int fd;
} FileBlockDevice;

static int file_read_block(BlockDevice* dev, block_t block, void* buffer) {
    FileBlockDevice* fbd = (FileBlockDevice*)dev->private_data;
    off_t offset = block * dev->block_size;
    if (lseek(fbd->fd, offset, SEEK_SET) < 0) return -1;
    return read(fbd->fd, buffer, dev->block_size) == (ssize_t)dev->block_size ? 0 : -1;
}

static int file_write_block(BlockDevice* dev, block_t block, const void* buffer) {
    FileBlockDevice* fbd = (FileBlockDevice*)dev->private_data;
    off_t offset = block * dev->block_size;
    if (lseek(fbd->fd, offset, SEEK_SET) < 0) return -1;
    return write(fbd->fd, buffer, dev->block_size) == (ssize_t)dev->block_size ? 0 : -1;
}

static int file_sync(BlockDevice* dev) {
    FileBlockDevice* fbd = (FileBlockDevice*)dev->private_data;
    return fsync(fbd->fd);
}

BlockDevice* block_device_open(const char* path, size_t block_size) {
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd < 0) return NULL;
    
    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        close(fd);
        return NULL;
    }
    
    BlockDevice* dev = malloc(sizeof(BlockDevice));
    FileBlockDevice* fbd = malloc(sizeof(FileBlockDevice));
    fbd->fd = fd;
    
    dev->private_data = fbd;
    dev->block_size = block_size;
    dev->total_blocks = size / block_size;
    dev->read_block = file_read_block;
    dev->write_block = file_write_block;
    dev->sync = file_sync;
    
    return dev;
}

void block_device_close(BlockDevice* dev) {
    if (!dev) return;
    FileBlockDevice* fbd = (FileBlockDevice*)dev->private_data;
    close(fbd->fd);
    free(fbd);
    free(dev);
}
