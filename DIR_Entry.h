#pragma once
#include <stdint.h>
#include <time.h>
typedef struct FileSystem FileSystem;

#define FILENAME_LEN 128 //lunghezza massima nome file
#define DIR_ENTRY_SIZE 32 
#define ENTRIES_PER_BLOCK (512 / DIR_ENTRY_SIZE) //512 sarebbe block size ma includendo fs_info mi dava errore...
typedef struct {
    char filename[15];
    uint8_t is_dir;         
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_block;
    uint32_t file_size;
} Dir_Entry; //struct di 32 byte
void create_Dir_Entry(Dir_Entry*, char*, uint16_t, int);
int fill_Dir_Entry(Dir_Entry*);
void free_Dir_Entry();
void print_Dir_Entry(Dir_Entry*);
uint16_t time_to_uint16(time_t);
void print_time(uint16_t );
uint16_t date_to_uint16(time_t timestamp);
void print_date(uint16_t date);
Dir_Entry* find_free_Dir_Entry(FileSystem*);

