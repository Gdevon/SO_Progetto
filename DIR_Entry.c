#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "DIR_Entry.h"
#include "Errors.h"
#include "FS_info.h"
#include "FAT_info.h"

void Dir_Entry_create(FileSystem* fs, Dir_Entry* free_entry, char* filename, uint16_t start, int type) {
    if (!fs || !free_entry || !filename) {
        return;
    }
    
    if (type == 0 && start < DATA_START_BLOCK) {
        return;
    }
    
    if (type == 0 && !fs->data) {
        print_error(DATA_NOTINIT);
        return;
    }
    
    if (type == 0) {
        uint32_t block_offset = (start - DATA_START_BLOCK) * BLOCK_SIZE;
        uint32_t max_offset = DATA_BLOCKS * BLOCK_SIZE;

        if (block_offset >= max_offset) {
            print_error(INVALID_BLOCK);
            return;
        }
        printf("Blocco %u valido, offset %u\n", start, block_offset);
    }
    
    memset(free_entry, 0, sizeof(Dir_Entry));
    memset(free_entry->filename,0,47);
    strncpy(free_entry->filename, filename,30);
    free_entry->filename[30] = '\0';
    free_entry->first_block = start;
    free_entry->is_dir = type;
    free_entry->file_size = 0;
    time_t now = time(NULL);
    free_entry->creation_time = time_to_uint16(now);
    free_entry->creation_date = date_to_uint16(now);
    free_entry->access_date = free_entry->creation_date;
    free_entry->modify_time = free_entry->creation_time;
    
    if (type == 1) {
        printf("Creata entry per file: %s\n", filename);
    } else {
        printf("Inizializzando directory '%s' nel blocco %u\n", filename, start);
        uint32_t block_offset = (start - DATA_START_BLOCK) * BLOCK_SIZE;
        Dir_Entry* dir_entries = (Dir_Entry*)(fs->data + block_offset);
        memset(fs->data + block_offset, 0, BLOCK_SIZE);   
        memset(&dir_entries[0],0,sizeof(Dir_Entry));  
        memset(dir_entries[0].filename,0,47);  
        strcpy(dir_entries[0].filename, ".");
        dir_entries[0].is_dir = 0;  
        dir_entries[0].first_block = start;
        dir_entries[0].file_size = 0;
        dir_entries[0].creation_time = time_to_uint16(now);
        dir_entries[0].creation_date = date_to_uint16(now);
        dir_entries[0].access_date = dir_entries[0].creation_date;
        dir_entries[0].modify_time = dir_entries[0].creation_time;
        memset(&dir_entries[1], 0, sizeof(Dir_Entry));
        memset(dir_entries[1].filename, 0, 47);
        strcpy(dir_entries[1].filename, "..");
        dir_entries[1].is_dir = 0;  
        dir_entries[1].first_block = fs->curr_dir;  
        dir_entries[1].file_size = 0;
        dir_entries[1].creation_time = time_to_uint16(now);
        dir_entries[1].creation_date = date_to_uint16(now);
        dir_entries[1].access_date = dir_entries[1].creation_date;
        dir_entries[1].modify_time = dir_entries[1].creation_time;
        printf("Directory inizializzata\n");
    }
}

void Dir_Entry_free(Dir_Entry* dh){}
void Dir_Entry_list(FileSystem* fs, uint16_t dir_block){
    if(!fs){
        print_error(FS_NOTINIT);
        return;
    }
    Dir_Entry* dir_entries;
    int max_entries;
    if(dir_block == ROOT_DIR_START_BLOCK){
        dir_entries = fs->root_dir;
        max_entries = ROOT_DIR_BLOCKS * ENTRIES_PER_BLOCK; //512 
        for(int i=0; i < max_entries;++i){
            Dir_Entry* entry = &dir_entries[i];
            Dir_Entry_list_aux(entry);
        }
    }else{
        uint16_t current = dir_block;
        while( current != FAT_BLOCK_END && current != FAT_BAD && current != FAT_FREE_BLOCK){
            uint32_t block_offset = (current-DATA_START_BLOCK) * BLOCK_SIZE;
            dir_entries = (Dir_Entry*)(fs->data+block_offset);
            max_entries = ENTRIES_PER_BLOCK;
            for(int i=0; i <max_entries;++i){
                Dir_Entry* de = &dir_entries[i];
                Dir_Entry_list_aux(de);
            }
            current = FAT_find_next_block(fs,current);
        }
    }
}
void Dir_Entry_list_aux(Dir_Entry* e){
    if (e->filename[0] != '\0') {
        int type = e->is_dir;
        if(type == 0){
            printf("<DIR>  Nome: %s  first_block=%u\n",
                    e->filename,
                    e->first_block);
        }else{
            int creation_h[3], creation_d[3];
            int access_d[3], modify_h[3];
            get_time(e->creation_time, creation_h);   
            get_date(e->creation_date, creation_d);   
            get_date(e->access_date, access_d);       
            get_time(e->modify_time, modify_h);       
            printf("<FILE> %uB  Nome: %s Primo blocco=%u\n"
                   "Data creazione: %02d-%02d-%04d  Orario creazione: %02d:%02d:%02d\n"
                   "Data ultimo accesso: %02d-%02d-%04d  Orario ultima modifica: %02d:%02d:%02d\n",
                   e->file_size,
                   e->filename,
                   e->first_block,
                   creation_d[0], creation_d[1], creation_d[2],
                   creation_h[0], creation_h[1], creation_h[2],
                   access_d[0], access_d[1], access_d[2],
                   modify_h[0], modify_h[1], modify_h[2]
            );
        }
    printf("-----------------------------\n"); 
    }
}
void Dir_Entry_curr_list(FileSystem* fs){
    Dir_Entry_list(fs,fs->curr_dir);
}
Dir_Entry* Dir_Entry_find_free(FileSystem* fs,uint16_t start_block){
    if(!fs){
        print_error(FS_NOTINIT);
        return NULL;
    }
    int max_entries;
    Dir_Entry* entries;
    if(start_block == ROOT_DIR_START_BLOCK|| start_block < DATA_START_BLOCK){
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
    if (dir_block == ROOT_DIR_START_BLOCK ){   
        dir_entries = fs->root_dir;  
        max_entries = ROOT_DIR_BLOCKS * ENTRIES_PER_BLOCK; 
    } else{
        if(dir_block < DATA_START_BLOCK){
            print_error(INVALID_BLOCK);
            return NULL;
        }
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
int Dir_Entry_change(FileSystem* fs, char* name){
    if(!fs){print_error(FS_NOTINIT);return -1;}
    if(!fs->mounted){print_error(DISK_UNMOUNTED);return -1;}
    if(name[0] == '\0'){print_error(EMPTY_NAME);return -1;}
    if (strcmp(name, "/") == 0) {
        fs->curr_dir = ROOT_DIR_START_BLOCK;
        printf("Cambiato in root directory\n");
        return 1;
    }    
    if (strcmp(name, "..") == 0) {
        if (fs->curr_dir == ROOT_DIR_START_BLOCK) {
            printf("Già in root directory\n");
            return 1;
        }
        Dir_Entry* entries;
        if (fs->curr_dir == ROOT_DIR_START_BLOCK) {
            entries = fs->root_dir;
        } else {
            uint32_t block_offset = (fs->curr_dir - DATA_START_BLOCK) * BLOCK_SIZE;
            entries = (Dir_Entry*)(fs->data + block_offset);
        }        
        if (strcmp(entries[1].filename, "..") == 0) {
            fs->curr_dir = entries[1].first_block;
            printf("Cambiato alla directory padre (blocco %u)\n", fs->curr_dir);
            return 1;
        } else {
            print_error(DIR_NOT_FOUND);
            return -1;
        }
    }
    Dir_Entry* target = Dir_Entry_find_name(fs, name, fs->curr_dir);
    if (!target) {
        print_error(DIR_NOT_FOUND);
        return -1;
    }
    if (target->is_dir == 1) { // 0 = directory
        print_error(NOT_A_DIR);
        return -1;
    }
    fs->curr_dir = target->first_block;
    //update_access_time(target);
    printf("Cambiato in directory '%s' (blocco %u)\n", name, fs->curr_dir);
    return 1;
}

uint16_t time_to_uint16(time_t timestamp) {
    struct tm *tm_info = localtime(&timestamp);
    return (uint16_t)(tm_info->tm_hour * 3600 + tm_info->tm_min * 60 + tm_info->tm_sec);
}
uint16_t date_to_uint16(time_t timestamp) {
    return (uint16_t)(timestamp / 86400);
}
void get_time(uint16_t t, int out[3]) {
    out[0] = t / 3600;
    out[1] = (t % 3600) / 60;
    out[2] = t % 60;
}
void get_date(uint16_t date, int out[3]) {
    time_t tt = (time_t)date * 86400;
    struct tm *tm_info = localtime(&tt);
    out[0] = tm_info->tm_mday;
    out[1] = tm_info->tm_mon + 1;
    out[2] = tm_info->tm_year + 1900;
}

void update_modify_time(Dir_Entry* entry) {
    if (!entry) return;
    time_t now = time(NULL);
    entry->modify_time = time_to_uint16(now);
}
void update_access_date(Dir_Entry* entry) {
    if (!entry) return;
    time_t now = time(NULL);
    entry->access_date = date_to_uint16(now);
}
