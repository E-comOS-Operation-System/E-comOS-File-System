#ifndef FS_TYPES_H
#define FS_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint64_t block_t;
typedef uint64_t inode_t;

#define FS_PATH_SEPARATOR '>'
#define FS_MAX_PATH_LEN 1000
#define FS_MAX_NAME_LEN 255

typedef enum {
    FS_TYPE_FILE = 1,
    FS_TYPE_FOLDER = 2
} fs_object_type_t;

typedef struct {
    uint64_t created;
    uint64_t modified;
    uint64_t accessed;
    uint64_t size;
    uint32_t permissions;
    uint32_t checksum;
} fs_metadata_t;

#endif
