#pragma once
#include <stdint.h>
#include <time.h>
#include "FS_info.h"
#include "FAT_info.h"

#define FILENAME_LEN 128 //lunghezza massima nome file
#define DIR_ENTRY_SIZE 32 
#define ENTRIES_PER_BLOCK (BLOCK_SIZE / DIR_ENTRY_SIZE)

typedef struct {
    char filename[16];         
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_block;
    uint32_t file_size;
} Dir_Entry;
