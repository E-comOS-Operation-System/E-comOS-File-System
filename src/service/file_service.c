#include "../../include/common/fs_types.h"
#include "../../include/efs/efs.h"
#include <stdint.h>

static EFS* mounted_fs = NULL;

void file_service_init(const char* device) {
    mounted_fs = efs_mount(device);
    if (!mounted_fs) {
        mounted_fs = efs_format(device);
    }
}

int handle_stat_request(const char* path, void* stat_buf) {
    if (!mounted_fs) return -1;
    return 0;
}

int handle_access_request(const char* path, int mode) {
    if (!mounted_fs) return -1;
    return 0;
}

int handle_unlink_request(const char* path) {
    if (!mounted_fs) return -1;
    return 0;
}
