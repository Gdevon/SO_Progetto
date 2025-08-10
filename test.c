#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "FAT_info.h"
#include "FS_info.h"
#include "FileHandle.h"
#include "FS_info.h"
#include "DIR_Entry.h"
#include "ListHandle.h"
#include "DirHandle.h"

int test_filehandle();
int test_duplicate_name();
int test_dirhandle();
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
    /*    puts("ALLOCO UN DIRENTRY E STAMPO A MANO");
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
        free_FileHandle(fh2);
        free_Dir_Entry(de);
        */
    //Testo creazione con NULL passato come parametro
    //Dir_Entry* de = make_Dir_Entry();
    //if(!de) return -1;
    //print_Dir_Entry(de);
    //FileHandle* fh = make_FileHandle((Dir_Entry*)NULL);
    //if(!fh) return -1;
    //Dir_Entry* de = make_Dir_Entry();
    //if(!de) printf("de non inizializzata correttamente\n");
    //fill_Dir_Entry(de);
    //print_Dir_Entry(de);
    //FileHandle* fh = make_FileHandle(de);
    //if(fill_FileHandle(fh) < 0) return 0;
    //printf("Prima free fh\n");
    //free_FileHandle(&fh);
    //printf("Seconda free fh\n");
    //free_FileHandle(&fh); //doppia free
    //free_Dir_Entry(de);
    //Dir_Entry* de = make_Dir_Entry();
    //if(fill_Dir_Entry(de) < 0 ) return 0;
    //FileHandle* handles[10];
    //for (int i = 0; i < 10; i++) {
    //    handles[i] = make_FileHandle(de);
    //    if (!handles[i] || !fill_FileHandle(handles[i])) {
    //        printf("Errore riempimento handle %d\n", i);
    //    }
    //}
    //for (int i = 0; i < 10; i++) {
    //    free_FileHandle(&handles[i]);
    //}
    //free_Dir_Entry(de);
    /*FileSystem* fs = fs_init();
    if(disk_creat("mydisk.fs", DISK_SIZE) < 0){
        return -1;
    }
    if(disk_mount(fs,"mydisk.fs") < 0){
        return -1;
    }
    printf("fs montato:  %d\n", fs->mounted);
    if(disk_mount(fs,"mydisk.fs") > 0){
        printf("Errore: ho appena montato di nuovo il fs su mydisk\n");
        return -1;
    }
    printf("bloccato tentativo di mount doppio\n");
    if(disk_unmount(fs) < 0){
        return -1;
    }
    if(disk_unmount(fs) > 0){
        printf("doppio unmount\n");
        return -1;
    }
    printf("bloccato tentativo di un,mount doppio\n");
    fs_free(&fs);
    return 1;*/
   int test;
    //test = test_filehandle();
    //if (test < 0) {
    //    printf("Test_filehandle fallito\n");
    //    return -1;
    //}
    //printf("Test_filehandle completato con successo!\n\n");
    test = test_dirhandle();
    if(test < 0 ) printf("Test dirhandle fallito");
    return 1;
}
int test_filehandle() {
    FileSystem* fs = fs_init();
    if (!fs) {
        return -1;
    }
    if (disk_creat("mydisk.fs", DISK_SIZE) < 0) {
        fs_free(&fs);
        return -1;
    }
    if (disk_mount(fs, "mydisk.fs") < 0) {
        fs_free(&fs);
        return -1;
    }

    FileHandle* fh1 = FileHandle_create(fs, "file1.txt");
    if (!fh1) {
        disk_unmount(fs);
        return -1;
    }
    FileHandle* fh2 = FileHandle_create(fs, "file2.txt");
    if (!fh2) {
        disk_unmount(fs);
        return -1;
    }
    if (fs->handles.size != 2) {
        printf("Errore: dimensione lista handles non corretta (%d, atteso 2)\n", fs->handles.size);
        disk_unmount(fs);
        return -1;
    }
    printf("Lista handles dopo creazione di due FileHandle:\n");
    List_print(&fs->handles);

    FileHandle_free(fs, fh1);
    if (fs->handles.size != 1) {
        printf("Errore: dimensione lista handles non corretta (%d, atteso 1)\n", fs->handles.size);
        disk_unmount(fs);
        return -1;
    }
    printf("Lista handles dopo chiusura di file1:\n");
    List_print(&fs->handles);

    FileHandle_free(fs, fh2);
    if (fs->handles.size != 0) {
        printf("Errore: dimensione lista handles non corretta (%d, atteso 0)\n", fs->handles.size);
        disk_unmount(fs);
        return -1;
    }
    printf("Lista handles dopo chiusura di file2:\n");
    List_print(&fs->handles);

    if (disk_unmount(fs) < 0) {
        return -1;
    }
    if (fs->handles.size != 0) {
        printf("Errore: lista handles non vuota dopo unmount (%d)\n", fs->handles.size);
        return -1;
    }
    printf("Lista handles dopo unmount:\n");
    List_print(&fs->handles);

    fs_free(&fs);
    printf("Test concluso con successo\n");
    return 1;
}
int test_dirhandle(){
    char disk[] = "mydisk.fs"; 
    FileSystem* fs = fs_init();
    if(!fs) return -1;
    if(disk_creat(disk,DISK_SIZE) < 0) return -1; 
    if(disk_mount(fs,disk) < 0) return -1;
    DirHandle* dh = DirHandle_create(fs,"lamiacartella");
    if(!dh) return -1;
    DirHandle *dh2 = DirHandle_create(fs,"lamiacartella2");
    if(!dh2) return -1;
    List_print(&fs->handles);
    if(disk_unmount(fs)<0) return -1;
    fs_free(&fs);
    return 1;
}