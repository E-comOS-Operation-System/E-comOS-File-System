CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = 

SRC_COMMON = src/common/block_device.c src/common/path.c
SRC_EFS = src/efs/efs_core.c src/efs/efs_file.c src/efs/efs_dir.c
SRC_NEFS = src/nefs/nefs_core.c src/nefs/nefs_file.c
SRC_WM2UD = src/wm2ud/wm2ud_core.c src/wm2ud/wm2ud_page.c
SRC_SERVICE = src/service/file_service.c
SRC_ALL = $(SRC_COMMON) $(SRC_EFS) $(SRC_NEFS) $(SRC_WM2UD) $(SRC_SERVICE)

OBJ_COMMON = $(SRC_COMMON:.c=.o)
OBJ_EFS = $(SRC_EFS:.c=.o)
OBJ_NEFS = $(SRC_NEFS:.c=.o)
OBJ_WM2UD = $(SRC_WM2UD:.c=.o)
OBJ_SERVICE = $(SRC_SERVICE:.c=.o)
OBJ_ALL = $(OBJ_COMMON) $(OBJ_EFS) $(OBJ_NEFS) $(OBJ_WM2UD) $(OBJ_SERVICE)

TARGET_LIB = libefs.a

all: $(TARGET_LIB)

test_efs: tests/test_efs.c $(TARGET_LIB)
	$(CC) $(CFLAGS) $< -L. -lefs -o $@

test_nefs: tests/test_nefs.c $(TARGET_LIB)
	$(CC) $(CFLAGS) $< -L. -lefs -o $@

test_wm2ud: tests/test_wm2ud.c $(TARGET_LIB)
	$(CC) $(CFLAGS) $< -L. -lefs -o $@

$(TARGET_LIB): $(OBJ_ALL)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_ALL) $(TARGET_LIB)

.PHONY: all clean
