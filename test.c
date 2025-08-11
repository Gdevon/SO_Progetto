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
int test_write_read();
int test_explore_block();
int test_filehandle_write();
int main(int argc, char* argv[]){
    int test;
    test = test_filehandle_write();
    if(test) printf("Fine test\n");
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
int test_write_read(){
    FileSystem* fs = fs_init();
    if(!fs) return -1;
    if(disk_creat("mydisk.fs",DISK_SIZE) < 0){
        fs_free(&fs);
        return -1;
    }
    if(disk_mount(fs,"mydisk.fs")<0){
        fs_free(&fs);
        return -1;
    }
    char msg[BLOCK_SIZE] = "Prova scrittura testo\0";
    uint16_t test_block = DATA_START_BLOCK + 10;
    if(fs_write_block(fs,test_block,msg) < 0){
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    char read_bytes[BLOCK_SIZE];
    if(fs_read_block(fs,test_block,read_bytes) < 0){
        printf("Lettura fallita\n");
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    printf(" Ho letto : %s\n",read_bytes);
    disk_unmount(fs);
    fs_free(&fs);
    return 1;
}
int test_explore_block(){
    FileSystem* fs = fs_init();
    if(!fs) return -1;
    if(disk_creat("mydisk.fs",DISK_SIZE) < 0){
        fs_free(&fs);
        return -1;
    }
    if(disk_mount(fs,"mydisk.fs")<0){
        fs_free(&fs);
        return -1;
    }
    char msg[BLOCK_SIZE] = "Prova scrittura testo\n Messaggio piu grande\t\t Ancora";
    uint16_t test_block = DATA_START_BLOCK + 10;
    if(fs_write_block(fs,test_block,msg) < 0){
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    char read_bytes[BLOCK_SIZE];
    if(fs_read_block(fs,test_block,read_bytes) < 0){
        printf("Lettura fallita\n");
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    printf(" Ho letto : %s\n",read_bytes);
    size_t off = fs_explore_block(fs,test_block);
    printf("offset di blocco ultimo carattere: %d\n",(int)off);
    disk_unmount(fs);
    fs_free(&fs);
    return 1;
}

int test_filehandle_write() {    
    FileSystem* fs = fs_init();
    if (!fs) {
        return -1;
    }
    
    if (disk_creat("test_write.fs", DISK_SIZE) < 0) {
        fs_free(&fs);
        return -1;
    }
    
    if (disk_mount(fs, "test_write.fs") < 0) {
        fs_free(&fs);
        return -1;
    }    
    FileHandle* fh1 = FileHandle_create(fs, "small.txt");
    if (!fh1) {
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
        
    char msg[] = "Messaggio di prova";
    size_t msg_size = strlen(msg);
    int result1 = FileHandle_write(fs, fh1, msg, msg_size);
    if (result1 < 0) {
        FileHandle_free(fs, fh1);
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }    
    printf("File size aggiornata: %u bytes\n", fh1->dir->file_size);
    printf("Byte offset corrente: %u\n", fh1->byte_offset);
    
    char verify_buffer[BLOCK_SIZE];
    if (fs_read_block(fs, fh1->dir->first_block, verify_buffer) < 0) {
        FileHandle_free(fs, fh1);
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    
    if (memcmp(msg, verify_buffer, msg_size) == 0) {
    } else {
        printf("Atteso: '%s'\n", msg);
        printf("Trovato: '%s'\n", verify_buffer);
        FileHandle_free(fs, fh1);
        disk_unmount(fs);
        fs_free(&fs);
        return -1;
    }
    FileHandle_free(fs, fh1);
    disk_unmount(fs);
    fs_free(&fs);
    return 1;
}
    