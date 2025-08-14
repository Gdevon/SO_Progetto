#pragma once
#include <stdint.h>
#include "DIR_Entry.h"
typedef struct {
    Dir_Entry* entries;    
    uint32_t num_entries;        
    uint32_t position;     
    uint16_t first_block;  
    uint8_t open;          
} DirHandle;

DirHandle* DirHandle_create(FileSystem* ,char*);
int DirHandle_close(FileSystem* , DirHandle* );
void DirHandle_free(FileSystem* , DirHandle* );
