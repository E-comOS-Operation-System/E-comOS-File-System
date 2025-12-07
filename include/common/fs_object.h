#ifndef FS_OBJECT_H
#define FS_OBJECT_H

#include "fs_types.h"

typedef struct FileObject {
    inode_t inode;
    char name[FS_MAX_NAME_LEN + 1];
    fs_metadata_t metadata;
    void* fs_private;
} FileObject;

typedef struct FolderObject {
    inode_t inode;
    char name[FS_MAX_NAME_LEN + 1];
    fs_metadata_t metadata;
    void* fs_private;
} FolderObject;

typedef struct {
    void* items;
    size_t count;
} List;

// Core operations
FileObject* create_file(const char* path, const void* data, size_t size);
FolderObject* create_folder(const char* path);
int delete_object(const char* path);
int rename_object(const char* old_path, const char* new_path);

FileObject* get_file(const char* path);
FolderObject* get_folder(const char* path);
List* list_objects(const char* path);

void free_file(FileObject* file);
void free_folder(FolderObject* folder);
void free_list(List* list);

#endif
