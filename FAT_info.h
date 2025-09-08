#pragma once
#include "FS_info.h"
#include "linked_list.h"
//costanti per la gestione della fat
#define FAT_ENTRY_SIZE 2  // entries a 16 bit
#define FAT_ENTRIES DATA_BLOCKS //numero di entries uguale al numero di blocchi totale 
#define FAT_FREE_BLOCK 0x0000
#define FAT_BLOCK_END 0xFFFF //fine catena blocchi file
#define FAT_BAD -2

uint16_t FAT_find_free_block(FileSystem* );
uint16_t FAT_find_next_block(FileSystem*, uint16_t);
int FAT_free_chain(FileSystem*, uint16_t);
