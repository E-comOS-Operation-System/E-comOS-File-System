#include "../include/efs/efs.h"
#include "../include/efs/efs_ops.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("=== EFS Test ===\n");
    
    // 格式化设备
    printf("Formatting device...\n");
    EFS* fs = efs_format("test.img");
    if (!fs) {
        printf("Failed to format\n");
        return 1;
    }
    printf("Format OK\n");
    
    // 创建文件
    printf("Creating file...\n");
    if (efs_create_file(fs, "root>test.txt") < 0) {
        printf("Failed to create file\n");
        return 1;
    }
    printf("File created\n");
    
    // 写入数据
    printf("Writing data...\n");
    const char* data = "Hello EFS!";
    ssize_t written = efs_write_file(fs, 1, data, strlen(data), 0);
    printf("Written %ld bytes\n", written);
    
    // 读取数据
    printf("Reading data...\n");
    char buf[100] = {0};
    ssize_t read = efs_read_file(fs, 1, buf, sizeof(buf), 0);
    printf("Read %ld bytes: %s\n", read, buf);
    
    // 卸载
    printf("Unmounting...\n");
    efs_unmount(fs);
    printf("Done!\n");
    
    return 0;
}
