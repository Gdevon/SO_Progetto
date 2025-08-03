#include "DIR_Entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
Dir_Entry* make_Dir_Entry(){
    Dir_Entry* de = (Dir_Entry*)malloc(sizeof(Dir_Entry));
    if(de) return de;
    perror("Errore di Allocazione Dir_Entry");
    return NULL;
}
int fill_Dir_Entry(Dir_Entry* de){
    if(de){
    de->access_date = (uint16_t) 20250802;
    de->creation_date = (uint16_t) 20250802;
    de->creation_time = (uint16_t) 20250802;
    de->file_size = (uint16_t) 20000;
    de->first_block = (0xFA);
    de->modify_date = (uint16_t) 20250802;
    de->modify_time = (uint16_t) 20250802;
    strcpy(de->filename,"nomeprova.txt");
    return 1;
    }
    else{
        puts("Impossibile riempire, de non presente");
        return 0;
    }
}
void print_Dir_Entry(Dir_Entry* de){
    if(de){
         printf("----- DIR ENTRY INFO -----\n");
        printf("Filename:       %s\n", de->filename);
        printf("Creation Time:  %u\n", de->creation_time);
        printf("Creation Date:  %u\n", de->creation_date);
        printf("Access Date:    %u\n", de->access_date);
        printf("Modify Time:    %u\n", de->modify_time);
        printf("Modify Date:    %u\n", de->modify_date);
        printf("First Block:    %u\n", de->first_block);
        printf("File Size:      %u bytes\n", de->file_size);
        printf("--------------------------\n");
    }else{
        puts("Impossibile stampare, de non esistente");
    }
}
void free_Dir_Entry(Dir_Entry* de){
    if(de){
        free(de);
    }
    de = NULL;
}