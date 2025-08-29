#pragma once
#include <stdint.h>
#include <time.h>
typedef struct FileSystem FileSystem;

#define DIR_ENTRY_SIZE 64
#define ENTRIES_PER_BLOCK 8 //(512 / DIR_ENTRY_SIZE) //512 sarebbe block size
typedef struct {
    char filename[43];
    uint16_t first_block;    
    uint32_t file_size;      
    uint16_t creation_date;  
    uint16_t modify_date;    
    uint16_t access_date;    
    uint16_t creation_time;  
    uint8_t is_dir; //1 == file 0 == dir
} Dir_Entry;
typedef enum{
    PERM_WRITE = 1,
    PERM_READ = 2,
    PERM_NO = 4,
    PERM_CREAT = 8,
    PERM_EXCL = 16
}Permission;
void Dir_Entry_create(FileSystem*, Dir_Entry*, char*, uint16_t, int);
int Dir_Entry_fill(Dir_Entry*);
void Dir_Entry_free();
void Dir_Entry_print(Dir_Entry*);
uint16_t time_to_uint16(time_t);
void print_time(uint16_t );
uint16_t date_to_uint16(time_t );
void print_date(uint16_t );
int check_duplicates(FileSystem* ,char*);
Dir_Entry* Dir_Entry_find_free(FileSystem*,uint16_t);
Dir_Entry* Dir_Entry_find_name(FileSystem*,char*,uint16_t);
void Dir_Entry_list(FileSystem*,uint16_t);
void Dir_Entry_list_aux();
void Dir_Entry_curr_list(FileSystem*);
int Dir_Entry_change(FileSystem*, char*);



