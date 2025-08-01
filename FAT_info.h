#pragma once
#include "FS_info.h"

//costanti per la gestione della fat
#define FAT_ENTRY_SIZE 2  // entries a 16 bit
#define FAT_ENTRIES DATA_BLOCKS //numero di entries uguale al numero di blocchi totale 
#define FAT_FREE_BLOCK 0x0000
#define FAT_BLOCK_END 0xFFFF //massimo rappresentabile a 16 bit
#define FAT_BAD -1