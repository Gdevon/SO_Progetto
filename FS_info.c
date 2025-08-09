#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Errors.h"
#include "FS_info.h"
#include "FAT_info.h"
#include "DIR_Entry.h"

int disk_creat(char* disk_name, uint32_t size){
    if(size != DISK_SIZE){
        printf("size [%u] non corrispondente a DISK_SIZE: [%u]\n",size,DISK_SIZE);
        return -1;
    }
    int fd = open(disk_name,O_RDWR|O_CREAT|O_EXCL|O_TRUNC,0666);
    if(fd < 0){
        print_error(DISK_CREAT_OPEN_FAIL);
        return DISK_CREAT_OPEN_FAIL;
    }
    if(ftruncate(fd,DISK_SIZE)<0){
        print_error(TRUNC_ERR);
        if(close(fd)<0){
            print_error(DISK_CREAT_CLOSE_FAIL);
            return DISK_CREAT_CLOSE_FAIL;
        }
        return TRUNC_ERR;
    }
    uint8_t* disk = (uint8_t*)mmap(NULL,DISK_SIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,fd,0);
    if(disk == MAP_FAILED){
        print_error(DISK_CREAT_MMAP_FAIL);
        if(close(fd) < 0 ){
            print_error(DISK_CREAT_CLOSE_FAIL);
            return DISK_CREAT_CLOSE_FAIL;
        }
        return DISK_CREAT_MMAP_FAIL;
    }
    uint16_t* fat = (uint16_t*)(disk + (BLOCK_SIZE*BOOT_SECTOR_BLOCKS));
    for(int i = 0; i < FAT_ENTRIES; ++i){
        fat[i] = FAT_FREE_BLOCK;
    }//INIZIALIZZO FAT
    Dir_Entry* ROOT_DIR = (Dir_Entry*)(disk+ (BLOCK_SIZE)*(BOOT_SECTOR_BLOCKS+FAT_BLOCKS));
    for(int i = 0; i < ENTRIES_PER_BLOCK*ROOT_DIR_BLOCKS;++i){
        ROOT_DIR[i].first_block = 0;
    }//INIZIALIZZATA ROOTDIR
    uint8_t* data = (uint8_t*)(disk+(BLOCK_SIZE*DATA_START_BLOCK));
    memset(data,0,BLOCK_SIZE*DATA_BLOCKS);//PULISCO SEZIONE DATI
    if(munmap(disk,DISK_SIZE) < 0){
        print_error(DISK_CREAT_MUNMAP_FAIL);
        return DISK_CREAT_MUNMAP_FAIL;
    }
    if(close(fd)<0){
        print_error(DISK_CREAT_CLOSE_FAIL);
        return DISK_CREAT_CLOSE_FAIL;
    }
    printf("Disk_creat ha avuto successo\n");
    return 1;
}
int disk_mount(FileSystem* fs, char* disk_n){
    if(fs->mounted == 1){
        print_error(DISK_MOUNTED);
        return DISK_MOUNTED;
    }
    int fd;
    if( (fd = open(disk_n,O_RDWR)) < 0){
        print_error(DISK_MOUNT_OPEN_FAIL);
        return(DISK_MOUNT_OPEN_FAIL);
    }
    uint8_t* disk =(uint8_t*) mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED){
        print_error(DISK_MOUNT_MMAP_FAIL);
        if(close(fd) < 0){
            print_error(DISK_MOUNT_CLOSE_FAIL);
            return DISK_MOUNT_CLOSE_FAIL;
        }
        return DISK_MOUNT_MMAP_FAIL;
    }
    fs->fd = fd;
    fs->disk = disk;
    fs->fat = (uint16_t*)(disk+(BLOCK_SIZE*BOOT_SECTOR_BLOCKS));//B_S_B è 1 inrealtà
    fs->root_dir = (Dir_Entry*)(disk+BLOCK_SIZE*(FAT_BLOCKS+BOOT_SECTOR_BLOCKS));
    fs->mounted = 1;
    List_init(&fs->handles);
    printf("Disk_Mount ha avuto successo\n");
    return 1;
}
int disk_unmount(FileSystem* fs){
    if(fs->mounted == 0){
        print_error(DISK_UNMOUNTED);
        return DISK_UNMOUNTED;
    }
    ListItem* current = fs->handles.first;
    while(current){
        ListItem *next = current->next;
        ListItem_destroy(current);
        current = next;
    }
    List_init(&fs->handles);
    if( (munmap(fs->disk,DISK_SIZE))<0){
        print_error(DISK_UNMOUNT_MUNMAP_FAIL);
        return DISK_UNMOUNT_MUNMAP_FAIL;
    }
    if( (close(fs->fd)) <0 ){
        print_error(DISK_UNMOUNT_CLOSE_FAIL);
        return DISK_UNMOUNT_CLOSE_FAIL;
    }
    fs->disk = NULL;
    fs->fat = NULL;
    fs->root_dir = NULL;
    fs->fd = -1;
    fs->mounted = 0;
    printf("Disk_Unmount ha avuto successo\n");
    return 1;
}
FileSystem* fs_init(){
    FileSystem* fs = (FileSystem*)malloc(sizeof(FileSystem));
    if(fs){
        fs->fd = -1;
        fs->data = NULL;
        fs->disk = NULL;
        fs->fat = NULL;
        fs->root_dir = NULL;
        fs->mounted = 0;
        return fs;
    }
    print_error(FS_ALLOC_FAIL);
    return NULL;
}
void fs_free(FileSystem** fs){
    if(*fs){
        (*fs)->fd = -1;
        (*fs)->data = NULL;
        (*fs)->disk = NULL;
        (*fs)->fat = NULL;
        (*fs)->root_dir = NULL;
        (*fs)->mounted = 0;
        free(*fs);
        printf("free fs\n");
    }
}