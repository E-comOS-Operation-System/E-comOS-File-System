#ifndef NEFS_OPS_H
#define NEFS_OPS_H

#include "nefs.h"

int nefs_create_file(NEFS* fs, const char* path);
int nefs_create_folder(NEFS* fs, const char* path);
ssize_t nefs_write_file(NEFS* fs, inode_t ino, const void* data, size_t len, size_t offset);
ssize_t nefs_read_file(NEFS* fs, inode_t ino, void* data, size_t len, size_t offset);
int nefs_delete(NEFS* fs, const char* path);
inode_t nefs_lookup(NEFS* fs, const char* path);

int nefs_journal_begin(NEFS* fs);
int nefs_journal_commit(NEFS* fs);
int nefs_journal_rollback(NEFS* fs);

#endif
