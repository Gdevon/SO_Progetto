#include <stdio.h>

#include "FAT_info.h"
#include "FS_info.h"

uint16_t FAT_find_free_block(FileSystem* fs){
    for(uint16_t i = 0;i<DATA_BLOCKS;++i){
        if(fs->fat[i] == FAT_FREE_BLOCK){
            return DATA_START_BLOCK + i;
        }
    }return FAT_BLOCK_END;
}
uint16_t FAT_find_next_block(FileSystem* fs, uint16_t block_id){
    if(!fs){
        print_error(FS_NOTINIT);
        return FAT_BAD;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return FAT_BAD;
    }
    if(block_id < DATA_START_BLOCK){
        print_error(INVALID_BLOCK);
        return FAT_BAD;
    }
    uint16_t fat_idx = block_id - DATA_START_BLOCK;
    if(fat_idx >= FAT_ENTRIES){
        print_error(INVALID_BLOCK);
        return FAT_BAD;
    }
    uint16_t next_idx = fs->fat[fat_idx];
    if(next_idx == FAT_BLOCK_END || next_idx == FAT_FREE_BLOCK || next_idx == FAT_BAD){
        return next_idx;
    }
    return DATA_START_BLOCK + next_idx;
}
int FAT_free_chain(FileSystem* fs, uint16_t first_block) {
    if (!fs) {
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if (first_block < DATA_START_BLOCK) {
        return 0; 
    }
    uint16_t curr_block = first_block;
    while (curr_block != FAT_BLOCK_END && curr_block != FAT_FREE_BLOCK && curr_block != FAT_BAD) {
        if(curr_block < DATA_START_BLOCK || curr_block >= (DATA_START_BLOCK+DATA_BLOCKS)){
            print_error(INVALID_BLOCK);
            return -1;
        }
        uint16_t fat_idx = curr_block - DATA_START_BLOCK;
        uint16_t next_block = fs->fat[fat_idx];
        fs->fat[fat_idx] = FAT_FREE_BLOCK;
        if(next_block != FAT_BLOCK_END && next_block != FAT_FREE_BLOCK && next_block != FAT_BAD){
            curr_block = DATA_START_BLOCK + next_block;
        }
        else curr_block = next_block;
    }
    return 1;
}