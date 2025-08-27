#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "DIR_Entry.h"
#include "Errors.h"
#include "FS_info.h"

void Dir_Entry_create(Dir_Entry* free_entry, char*filename, uint16_t start, int type){
    if(type == 1){
        printf("Sto creando una entry per un file\n");
    }
    else{
        printf("Sto creando una entry per una dir\n");
    }
    memset(free_entry,0,sizeof(Dir_Entry));
    strcpy(free_entry->filename, filename);
    free_entry -> filename[30] = '\0';
    free_entry->first_block = start;
    free_entry->file_size = 0;
    free_entry->is_dir = type;
    //setto l'orario -- presa da stackoverflow
    time_t now = time(NULL);
    free_entry->creation_time = time_to_uint16(now);
    //uguale per la data
    free_entry->creation_date = date_to_uint16(now);
    free_entry->access_date = free_entry->creation_date;
    free_entry->modify_date = free_entry->creation_date;
}


void Dir_Entry_free(Dir_Entry* de){
    if(de){
        free(de);
    }
    de = NULL;
}

uint16_t time_to_uint16(time_t timestamp) {
    struct tm *tm_info = localtime(&timestamp);
    return (uint16_t)(tm_info->tm_hour * 3600 + tm_info->tm_min * 60 + tm_info->tm_sec);
}
void print_time(uint16_t t){
    int ore = t / 3600;
    int minuti = (t % 3600) / 60;
    int secondi = t % 60;
    printf("Orario: %02d:%02d:%02d\n",ore,minuti,secondi);
}

uint16_t date_to_uint16(time_t timestamp) {
    return (uint16_t)(timestamp / 86400);
}
void print_date(uint16_t date){
    time_t t = (time_t)date*86400;
    struct tm *tm_info = localtime(&t);
    printf("%02d-%02d-%04d",tm_info->tm_mon + 1, tm_info->tm_mday,tm_info->tm_year + 1900);
}

int is_dir(char* name){
    if(strstr(name,".")) return 1; // è un file se contiene .
    else return 0; //cartella se non contiene .
}
void Dir_Entry_list(FileSystem* fs, uint16_t dir_block){
    if(!fs){
        print_error(FS_NOTINIT);
        return;
    }
    Dir_Entry* dir_entries;
    int max_entries;
    if(dir_block == 0 || dir_block < DATA_START_BLOCK){
        dir_entries = fs->root_dir;
        max_entries = ROOT_DIR_BLOCKS * ENTRIES_PER_BLOCK; //512 
    }else{
        int32_t block_offset = (dir_block - DATA_START_BLOCK) * BLOCK_SIZE;
        dir_entries = (Dir_Entry*)(fs->data + block_offset);  
        max_entries = ENTRIES_PER_BLOCK;  //16
    }
    for (int i = 0; i < max_entries; i++) {
        Dir_Entry* e = &dir_entries[i];
        
        if (e->filename[0] != '\0') {
            printf("%s %uB  %s  first_block=%u\n", 
                   e->is_dir == 0 ? "<DIR>" : "<FILE>",  
                   e->file_size,                    
                   e->filename,                     
                   e->first_block);                 
        }
    }
    
    printf("-----------------------------\n");
}
Dir_Entry* Dir_Entry_find_free(FileSystem* fs,uint16_t start_block){
    int max_entries;
    Dir_Entry* entries;
    if(start_block == 0 || start_block < DATA_START_BLOCK){
        entries = fs->root_dir;
        max_entries = ROOT_DIR_BLOCKS*ENTRIES_PER_BLOCK;
    }
    else{
        uint32_t block_offset = (start_block - DATA_START_BLOCK)*BLOCK_SIZE;
        entries = (Dir_Entry*)(fs->data+block_offset);
        max_entries = ENTRIES_PER_BLOCK;
    }
    for(int i = 0;i<max_entries;++i){
        if(entries[i].filename[0] == '\0'){
            return &entries[i];
        }
    }
    print_error(NO_FREE_ENTRY);
    return NULL;
}
Dir_Entry* Dir_Entry_find_name(FileSystem* fs, char* name, uint16_t dir_block) {
    if (!fs) {
        print_error(FS_NOTINIT);
        return NULL;
    }
    
    Dir_Entry* dir_entries;
    int max_entries;
    if (dir_block == 0 || dir_block < DATA_START_BLOCK) {
        dir_entries = fs->root_dir;  
        max_entries = ROOT_DIR_BLOCKS * ENTRIES_PER_BLOCK; 
    } 
    else {
        uint32_t block_offset = (dir_block - DATA_START_BLOCK) * BLOCK_SIZE;
        dir_entries = (Dir_Entry*)(fs->data + block_offset);  
        max_entries = ENTRIES_PER_BLOCK;  
    }
    for (int i = 0; i < max_entries; i++) {
        if (dir_entries[i].filename[0] != '\0') {
            if (strcmp(dir_entries[i].filename, name) == 0) {
                return &dir_entries[i];  
            }
        }
    }
    return NULL;  // Non trovata
}
