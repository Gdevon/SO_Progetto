#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "FAT_info.h"
#include "DIR_Entry.h"
typedef enum{
    PERM_WRITE = 1,
    PERM_READ = 2,
    PERM_APP = 3,
    PERM_NO = 0
}Permission;
typedef struct{
    Dir_Entry* dir;
    uint32_t byte_offset;
    uint8_t open; //1 == OPEN, 0 == CLOSE;
    Permission permission;
}FileHandle;

FileHandle* FileHandle_create(FileSystem *, char* );
void FileHandle_free(FileSystem* ,FileHandle*);
void FileHandle_print(FileHandle*);
int is_dir(char*);
int FileHandle_write(FileSystem*, FileHandle* , char* ,size_t );
