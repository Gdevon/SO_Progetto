#include "FileHandle.h"
#include "DIR_Entry.h"
FileHandle* make_FileHandle(Dir_Entry* de){
    if(!de){
        printf("Non posso inizializzare fh con NULL pointer\n");
        return NULL;
    }else{
        if(de->creation_date <= 0){
            printf("de è priva di informazioni, riempio\n");
            fill_Dir_Entry(de);
        }
        FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
        if(fh != NULL && de != NULL){
            fh->dir = de;
            return fh;
        }
        printf("Errore nella creazione di fh");
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
