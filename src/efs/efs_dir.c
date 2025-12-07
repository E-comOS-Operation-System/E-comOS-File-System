#include "../../include/efs/efs.h"
#include "../../include/efs/efs_ops.h"
#include <string.h>

static uint64_t get_folder_size(EFS* fs, inode_t ino) {
    efs_inode_t inode;
    block_t inode_block = fs->superblock.inode_table_block + ino / (EFS_BLOCK_SIZE / sizeof(efs_inode_t));
    fs->device->read_block(fs->device, inode_block, &inode);
    return inode.size;
}

int efs_create_folder(EFS* fs, const char* path) {
    if (!fs || !path) return -1;
    if (strlen(path) > FS_MAX_PATH_LEN) return -1;
    
    return 0;
}

int efs_add_to_folder(EFS* fs, inode_t parent_ino, const char* name, inode_t child_ino) {
    uint64_t folder_size = get_folder_size(fs, parent_ino);
    if (folder_size >= EFS_MAX_FOLDER_SIZE) return -1;
    
    return 0;
}

inode_t efs_lookup(EFS* fs, const char* path) {
    if (!fs || !path) return (inode_t)-1;
    if (strlen(path) > FS_MAX_PATH_LEN) return (inode_t)-1;
    
    return 0;
}
