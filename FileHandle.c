#include "FileHandle.h"
#include "DIR_Entry.h"
FileHandle* make_FileHandle(Dir_Entry* de){
    FileHandle* fh = (FileHandle*) malloc(sizeof(FileHandle));
    if(fh && de){
        fh->dir = de;
        return fh;
    }else{
        perror("Non posso inserire de all'interno di fh");
        return NULL;
    }
}
int fill_FileHandle(FileHandle* fh){
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
}
void free_FileHandle(FileHandle* fh){
    if(fh){
        fh->byte_offset = 0;
        fh->open = 0;
        fh->permission = 0;
        free(fh);
        fh = NULL;
    }else{
        printf("Impossibile fare free di fh vuota");
    }
}
void print_FileHandle(FileHandle* fh){
        printf("INFORMAZIONI FILEHANDLE %s\n", fh->dir->filename);
        print_Dir_Entry(fh->dir);
        printf("Byte offset: %u\n", fh->byte_offset);
        printf("Open : %u\n", fh->open);
        printf("Permission: %u\n", fh->permission);
        printf("----------------\n");
}