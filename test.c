#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FAT_info.h"
#include "FS_info.h"
#include "FileHandle.h"
#include "DirHandle.h"
#include "DIR_Entry.h"
int main(int argc, char* argv[]){
    /*    puts("TEST DIMENSIONI ");
        printf(" BLOCK_SIZE: %d\n", SIZE_BLOCK);
        printf(" DIR_ENTRY_SIZE: %d\n", DIR_ENTRY_SIZE);
        printf(" ENTRIES_PER_BLOCK: %d\n", TOTAL_BLOCKS);
        printf(" TOTAL_BLOCKS: %d\n", TOTAL_BLOCKS);
        printf(" DATA_BLOCKS: %d\n", DATA_BLOCKS);
        printf(" FAT_BLOCKS: %d\n", FAT_BLOCKS);
        printf(" FAT_ENTRIES possibili (DATA_BLOCKS): %d\n", DATA_BLOCKS);
        printf("DIMENSIONE DIR ENTRY: %zu bytes, DEVE ESSERE : %d", sizeof(Dir_Entry), DIR_ENTRY_SIZE);
        puts("");
    */
        puts("ALLOCO UN DIRENTRY E STAMPO A MANO");
        Dir_Entry* de = make_Dir_Entry();
        if(fill_Dir_Entry(de) <= 0){
            perror("Errore riempimento campi de");
            return 0;
        }
        print_Dir_Entry(de);

        FileHandle* fh = make_FileHandle(de);
        if(fill_FileHandle(fh) == 0){
            perror("Errore riempimento campi fh");
            return 0;
        }
        print_FileHandle(fh);
        //riassegno la direntry ad un nuovo fh
        FileHandle* fh2 = make_FileHandle(de);
        if(fill_FileHandle(fh2) <0){
            perror("Errore riempimento campi fh");
            return 0;
        }
        print_FileHandle(fh2);
        free_FileHandle(fh);
        free_Dir_Entry(de);
    }