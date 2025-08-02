#pragma once
#include <stdint.h>
#include "FS_info.h"
#include "FAT_info.h"
#include "DIR_info.h"
typedef struct {
    Dir_Entry* entries;    
    uint32_t num_entries;        
    uint32_t position;     
    uint16_t first_block;  
    uint8_t open;          
} DirectoryHandle;