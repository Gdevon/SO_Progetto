#include "DIR_info.h"
#include "FAT_info.h"
#include "FS_info.h"
#include "FileHandle.h"
#include "DirHandle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]){
        puts("TEST DIMENSIONI ");
        printf(" BLOCK_SIZE: %d\n", SIZE_BLOCK);
        printf(" DIR_ENTRY_SIZE: %d\n", DIR_ENTRY_SIZE);
        printf(" ENTRIES_PER_BLOCK: %d\n", TOTAL_BLOCKS);
        printf(" TOTAL_BLOCKS: %d\n", TOTAL_BLOCKS);
        printf(" DATA_BLOCKS: %d\n", DATA_BLOCKS);
        printf(" FAT_BLOCKS: %d\n", FAT_BLOCKS);
        printf(" FAT_ENTRIES possibili (DATA_BLOCKS): %d\n", DATA_BLOCKS);
        printf("DIMENSIONE DIR ENTRY: %zu bytes, DEVE ESSERE : %d", sizeof(Dir_Entry), DIR_ENTRY_SIZE);
        puts("");
        puts("ALLOCO UN DIRENTRY E STAMPO A MANO");
        Dir_Entry* de = (Dir_Entry*) malloc(sizeof(Dir_Entry));
        if(!de) perror("Errore nell'inizializzazione di de");
        de->access_date = (uint16_t) 20250802;
        de->creation_date = (uint16_t) 20250802;
        de->creation_time = (uint16_t) 20250802;
        de->file_size = (uint16_t) 20000;
        de->first_block = (0xFA);
        de->modify_date = (uint16_t) 20250802;
        de->modify_time = (uint16_t) 20250802;
        strcpy(de->filename,"nomeprova.txt");
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
    }