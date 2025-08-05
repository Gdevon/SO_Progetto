#pragma once
#include <stdint.h>
#include <time.h>
#define FILENAME_LEN 128 //lunghezza massima nome file
#define DIR_ENTRY_SIZE 32 
#define ENTRIES_PER_BLOCK (512 / DIR_ENTRY_SIZE) //512 sarebbe block size ma includendo fs_info mi dava errore...

typedef struct {
    char filename[16];         
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_block;
    uint32_t file_size;
} Dir_Entry; //struct di 32 byte
Dir_Entry* make_Dir_Entry();
int fill_Dir_Entry(Dir_Entry*);
void free_Dir_Entry();
void print_Dir_Entry(Dir_Entry*);