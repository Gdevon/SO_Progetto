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
uint16_t FAT_find_next_block(FileSystem* fs, uint16_t block_id){
    if(!fs || !fs->mounted || block_id >= FAT_ENTRIES){
        print_error(INVALID_BLOCK);
        return FAT_BAD;
    }
    return fs->fat[block_id];
}
