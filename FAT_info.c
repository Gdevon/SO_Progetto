#include <stdio.h>

#include "FAT_info.h"
#include "FS_info.h"

uint16_t FAT_find_free_block(FileSystem* fs){
    for(uint16_t i = 0;i<FAT_ENTRIES;++i){
        if(fs->fat[i] == FAT_FREE_BLOCK){
            return i;
        }
    }return FAT_BLOCK_END;
}
