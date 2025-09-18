#pragma once
#include "FS_info.h"
//costanti per la gestione della fat
#define FAT_ENTRIES DATA_BLOCKS  
#define FAT_FREE_BLOCK 0x0000
#define FAT_BLOCK_END 0xFFFF 
#define FAT_BAD 0xFFFE

uint16_t FAT_find_free_block(FileSystem* );
uint16_t FAT_find_next_block(FileSystem*, uint16_t);
int FAT_free_chain(FileSystem*, uint16_t);
