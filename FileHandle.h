#pragma once
#include "FS_info.h"
#include "FAT_info.h"
#include "DIR_info.h"
#include <cstdint>
typedef enum{
    PERM_WRITE = 1,
    PERM_READ = 2,
    PERM_APP = 3,
    PERM_NO = 0
}Permission;
typedef struct{
    Dir_Entry dir;
    uint32_t byte_offset;
    uint8_t open; //1 == OPEN, 0 == CLOSE;
    Permission permission;
}FileHandle;