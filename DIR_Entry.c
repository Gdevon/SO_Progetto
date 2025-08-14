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
        printf("Sto creando una entry per una dir");
    }
    memset(free_entry,0,sizeof(Dir_Entry));
    strcpy(free_entry->filename, filename);
    free_entry -> filename[14] = '\0';
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
Dir_Entry* Dir_Entry_find_free(FileSystem* fs){
    for(int i=0; i<ROOT_DIR_BLOCKS*ENTRIES_PER_BLOCK;++i){
        if(fs->root_dir[i].file_size == 0){
            return &fs->root_dir[i];
        }
    }
    print_error(NO_FREE_ENTRY); 
    return NULL;
}
void Dir_Entry_print(Dir_Entry* de){
    if(de && de->access_date > 0){
        printf("----- DIR ENTRY INFO -----\n");
        printf("Filename:       %s\n", de->filename);
        printf("Creation Time:  %u\n", de->creation_time);
        printf("Creation Date:  %u\n", de->creation_date);
        printf("Access Date:    %u\n", de->access_date);
        //printf("Modify Time:    %u\n", de->modify_time);
        printf("Modify Date:    %u\n", de->modify_date);
        printf("First Block:    %u\n", de->first_block);
        printf("File Size:      %u bytes\n", de->file_size);
        printf("--------------------------\n");
    }else{
        puts("Impossibile stampare, de non esistente o vuota");
    }
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
int check_duplicates(FileSystem* fs,char* filename){
    for(int i = 0;i<ROOT_DIR_BLOCKS*ENTRIES_PER_BLOCK;++i){
        if(fs->root_dir[i].filename[0] != '\0'){
            if(strcmp(fs->root_dir[i].filename,filename) == 0){
                return -1;
            }
        }
    }
    return 1;
}