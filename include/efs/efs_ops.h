#ifndef EFS_OPS_H
#define EFS_OPS_H

#include "efs.h"

int efs_create_file(EFS* fs, const char* path);
int efs_create_folder(EFS* fs, const char* path);
int efs_add_to_folder(EFS* fs, inode_t parent_ino, const char* name, inode_t child_ino);
ssize_t efs_write_file(EFS* fs, inode_t ino, const void* data, size_t len, size_t offset);
ssize_t efs_read_file(EFS* fs, inode_t ino, void* data, size_t len, size_t offset);
int efs_delete(EFS* fs, const char* path);
inode_t efs_lookup(EFS* fs, const char* path);

#endif
