#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "FAT_info.h"
#include "DIR_Entry.h"
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
typedef struct{
    Dir_Entry* dir;
    uint32_t byte_offset;
    uint8_t open; //1 == OPEN, 0 == CLOSE;
    Permission permission;
}FileHandle;

FileHandle* FileHandle_open(FileSystem *, char*,Permission);
int FileHandle_close(FileSystem* ,FileHandle*);
void FileHandle_free(FileSystem* , FileHandle* );
void FileHandle_print(FileHandle*);
int FileHandle_write(FileSystem*, FileHandle* , char* ,size_t );
int FileHandle_read(FileSystem*, FileHandle*, char*,size_t);
int FileHandle_seek(FileHandle*,int,int);
int FileHandle_tell(FileHandle*);
int FileHandle_delete(FileSystem*, char*);