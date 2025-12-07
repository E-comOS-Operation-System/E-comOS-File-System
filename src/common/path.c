#include "../../include/common/fs_types.h"
#include <string.h>
#include <stdlib.h>

char** split_path(const char* path, size_t* count) {
    if (!path || !count) return NULL;
    
    size_t len = strlen(path);
    if (len > FS_MAX_PATH_LEN) return NULL;
    
    char* temp = strdup(path);
    *count = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (temp[i] == FS_PATH_SEPARATOR) (*count)++;
    }
    (*count)++;
    
    char** parts = malloc(sizeof(char*) * (*count));
    char* token = strtok(temp, ">");
    size_t idx = 0;
    
    while (token && idx < *count) {
        parts[idx++] = strdup(token);
        token = strtok(NULL, ">");
    }
    
    free(temp);
    *count = idx;
    return parts;
}

void free_path_parts(char** parts, size_t count) {
    if (!parts) return;
    for (size_t i = 0; i < count; i++) {
        free(parts[i]);
    }
    free(parts);
}
