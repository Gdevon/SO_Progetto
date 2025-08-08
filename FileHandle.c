#include <stdio.h>
#include <string.h>
#include "Errors.h"
#include "FileHandle.h"
#include "DIR_Entry.h"
#include "FS_info.h"

FileHandle* create_FileHandle(FileSystem* fs, char* filename){
    if(fs->mounted == 0){
        print_error(DISK_UNMOUNTED);
        return NULL;
    }
    if(check_filename(filename) < 0){
        print_error(LONG_NAME);
        return NULL;
    }
    // da fare:devo controllare se il file esiste gia -check_duplicates()
    Dir_Entry* free_entry = find_free_Dir_Entry(fs);
    if(!free_entry){
        print_error(FULL_DIR);
        return NULL;
    }
    uint16_t free_fat = find_free_FAT_block(fs);
    if(free_fat == FAT_BLOCK_END){
        print_error(FULL_FAT);
        return NULL;
    }
    fs->fat[free_fat] = FAT_BLOCK_END;
    create_Dir_Entry(free_entry,filename,free_fat,is_dir(filename));
    FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
    if(!fh){
        print_error(FH_ALLOC_FAIL);
        return NULL;
    }
    fh->dir = free_entry;
    fh->byte_offset = 0;
    fh->open = 1;
    fh->permission = PERM_WRITE;
    return fh;
}


/*int fill_FileHandle(FileHandle* fh){
    if(fh){
        fh->byte_offset = 1020; //valori random
        fh->open = 1;
        fh->permission = 3;
        return 1;
    }
    else{
        printf("Impossibile riempire fh vuota\n");
        return 0;
    }
}*/
void free_FileHandle(FileHandle** fh){
    if(*fh && fh){
        //free_Dir_Entry((*fh)->dir);
        (*fh)->byte_offset = 0;
        (*fh)->open = 0;
        (*fh)->permission = 0;
        free(*fh);
        *fh = NULL;
        printf("free effettuata correttamente\n");
    }else{
        printf("Impossibile fare free di fh vuota\n");
        return;
    }
}
void print_FileHandle(FileHandle* fh){
    if(fh->open == 1){
        printf("INFORMAZIONI FILEHANDLE %s\n", fh->dir->filename);
        printf("Byte offset: %u\n", fh->byte_offset);
        printf("Open : %u\n", fh->open);
        printf("Permission: %u\n", fh->permission);
        printf("----------------\n");
        print_Dir_Entry(fh->dir);
    }
    else{
        printf("FileHandle non aperto\n");
    }
}
int is_dir(char* filename){
    if(strstr(filename,".")) return 1; // è un file se contiene .
    else return 0; //cartella se non contiene .
}
int check_filename(char* filename){
    if(strlen(filename) > 14){
        return -1;
    }else{
        return 1;
    }
}
