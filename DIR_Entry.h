#pragma once
#include <stdint.h>
#include <time.h>
typedef struct FileSystem FileSystem;

#define DIR_ENTRY_SIZE 32 
#define ENTRIES_PER_BLOCK (512 / DIR_ENTRY_SIZE) //512 sarebbe block size
typedef struct {
    char filename[31];
    uint8_t is_dir;         //1 == file 0 == dir
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t modify_date;
    uint16_t first_block;
    uint32_t file_size;
} Dir_Entry; //struct di 32 byte
void Dir_Entry_create(Dir_Entry*, char*, uint16_t, int);
int Dir_Entry_fill(Dir_Entry*);
void Dir_Entry_free();
void Dir_Entry_print(Dir_Entry*);
Dir_Entry* Dir_Entry_find_free(FileSystem*);
uint16_t time_to_uint16(time_t);
void print_time(uint16_t );
uint16_t date_to_uint16(time_t );
void print_date(uint16_t );
int check_duplicates(FileSystem* ,char*);
int is_dir(char*);
Dir_Entry* Dir_Entry_find(FileSystem*, char*);


