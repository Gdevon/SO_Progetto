#pragma once
#include <stdint.h>
#include "Errors.h"
#include "DIR_Entry.h"
#include "linked_list.h"
#define BLOCK_SIZE 512                //512byte per ogni blocco
#define DISK_SIZE (32 * 1024 * 1024)  //32mb didisco
#define TOTAL_BLOCKS (DISK_SIZE / BLOCK_SIZE) 

//divisione del fs |reserved(1 solo blocco)|fat|root|data|
#define BOOT_SECTOR_BLOCKS 1           
#define FAT_BLOCKS 256                //ogni blocco di fat puo contenere fino a 256 indici di blocchi di file: essendo un blocco da 512 bytes e visto che un indirizzo richiede 2 bytes al più per essere memorizzato
#define ROOT_DIR_BLOCKS 32            
#define DATA_START_BLOCK (BOOT_SECTOR_BLOCKS + FAT_BLOCKS + ROOT_DIR_BLOCKS) 
#define DATA_BLOCKS (TOTAL_BLOCKS - DATA_START_BLOCK)

//struct Dir_Entry Dir_Entry;
struct FileSystem{
    int fd;
    uint8_t* disk;
    uint16_t* fat;
    Dir_Entry* root_dir;
    uint8_t* data;
    uint8_t mounted;
    ListHead handles;
};
typedef struct FileSystem FileSystem;
int disk_creat(char* disk_name,uint32_t size);
int disk_mount(FileSystem* fs,char* disk_name);
int disk_unmount(FileSystem*);
FileSystem* fs_init();
void fs_free(FileSystem**);
int fs_write_block(FileSystem*,uint16_t,char*);
int fs_read_block(FileSystem*,uint16_t,char*);
size_t fs_explore_block(FileSystem*, uint16_t);