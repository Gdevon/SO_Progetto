#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "FS_info.h"
#include "FAT_info.h"
#include "DIR_Entry.h"
#define TRUNC_ERR -1
#define DISK_CREAT_CLOSE_FAIL -2
#define DISK_CREAT_OPEN_FAIL -3
#define DISK_CREAT_MUNMAP_FAIL -4
#define DISK_CREAT_MMAP_FAIL -5
#define DISK_MOUNT_OPEN_FAIL -6
#define DISK_MOUNT_MMAP_FAIL -7
#define DISK_UNMOUNT_MUNMAP_FAIL -8
#define DISK_UNMOUNT_CLOSE_FAIL -9
#define DISK_MOUNTED -10
#define DISK_MOUNT_CLOSE_FAIL -11
#define DISK_UNMOUNTED -12
void disk_op_error_print(int err) {
    switch (err) {
        case TRUNC_ERR:
            printf("Errore: ftruncate non ha avuto successo\n");
            break;
        case DISK_CREAT_CLOSE_FAIL:
            printf("Errore: close non ha avuto successo\n");
            break;
        case DISK_CREAT_OPEN_FAIL:
            printf("Errore: apertura del file in creat non riuscita\n");
            break;
        case DISK_CREAT_MUNMAP_FAIL:
            printf("Errore: unmapping del disco in creat non riuscito\n");
            break;
        case DISK_CREAT_MMAP_FAIL:
            printf("Errore: mmap del disco in creat non riuscita\n");
            break;
        case DISK_MOUNT_OPEN_FAIL:
            printf("Errore: apertura file in mount non riuscita\n");
            break;
        case DISK_MOUNT_MMAP_FAIL:
            printf("Errore: mmap durante il mount non riuscito\n");
            break;
        case DISK_UNMOUNT_MUNMAP_FAIL:
            printf("Errore: unmapping durante l'unmount non riuscito\n");
            break;
        case DISK_UNMOUNT_CLOSE_FAIL:
            printf("Errore: close durante l'unmount non riuscito\n");
            break;
        case DISK_MOUNTED:
            printf("Errore: il file system è già montato\n");
            break;
        case DISK_MOUNT_CLOSE_FAIL:
            printf("Errore nella chiusura del file in mount\n");
            break;
        default:
            printf("Errore sconosciuto: %d\n", err);
            break;
    }
}
int disk_creat(char* disk_name, uint32_t size){
    if(size != DISK_SIZE){
        printf("size [%u] non corrispondente a DISK_SIZE: [%u]\n",size,DISK_SIZE);
        return -1;
    }
    int fd = open(disk_name,O_RDWR|O_CREAT|O_EXCL|O_TRUNC,0666);
    if(fd < 0){
        disk_op_error_print(DISK_CREAT_OPEN_FAIL);
        return DISK_CREAT_OPEN_FAIL;
    }
    if(ftruncate(fd,DISK_SIZE)<0){
        disk_op_error_print(TRUNC_ERR);
        if(close(fd)<0){
            disk_op_error_print(DISK_CREAT_CLOSE_FAIL);
            return DISK_CREAT_CLOSE_FAIL;
        }
        return TRUNC_ERR;
    }
    uint8_t* disk = (uint8_t*)mmap(NULL,DISK_SIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,fd,0);
    if(disk == MAP_FAILED){
        disk_op_error_print(DISK_CREAT_MMAP_FAIL);
        if(close(fd) < 0 ){
            disk_op_error_print(DISK_CREAT_CLOSE_FAIL);
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
        disk_op_error_print(DISK_CREAT_MUNMAP_FAIL);
        return DISK_CREAT_MUNMAP_FAIL;
    }
    if(close(fd)<0){
        disk_op_error_print(DISK_CREAT_CLOSE_FAIL);
        return DISK_CREAT_CLOSE_FAIL;
    }
    return 1;
}
int disk_mount(FileSystem* fs, char* disk_n){
    if(fs->mounted == 1){
        disk_op_error_print(DISK_MOUNTED);
        return DISK_MOUNTED;
    }
    int fd;
    if( (fd = open(disk_n,O_RDWR)) < 0){
        disk_op_error_print(DISK_MOUNT_OPEN_FAIL);
        return(DISK_MOUNT_OPEN_FAIL);
    }
    uint8_t* disk =(uint8_t*) mmap(NULL, DISK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED){
        disk_op_error_print(DISK_MOUNT_MMAP_FAIL);
        if(close(fd) < 0){
            disk_op_error_print(DISK_MOUNT_CLOSE_FAIL);
            return DISK_MOUNT_CLOSE_FAIL;
        }
        return DISK_MOUNT_MMAP_FAIL;
    }
    fs->fd = fd;
    fs->disk = disk;
    fs->fat = (uint16_t*)(disk+(BLOCK_SIZE*BOOT_SECTOR_BLOCKS));//B_S_B è 1 inrealtà
    fs->root_dir = (Dir_Entry*)(disk+BLOCK_SIZE*(FAT_BLOCKS+BOOT_SECTOR_BLOCKS));
    fs->mounted = 1;
    return 1;
}
int disk_unmount(FileSystem* fs){
    if(fs->mounted == 0){
        disk_op_error_print(DISK_UNMOUNTED);
        return DISK_MOUNTED;
    }
    if( (munmap(fs->disk,DISK_SIZE))<0){
        disk_op_error_print(DISK_UNMOUNT_MUNMAP_FAIL);
        return DISK_UNMOUNT_MUNMAP_FAIL;
    }
    if( (close(fs->fd)) <0 ){
        disk_op_error_print(DISK_UNMOUNT_CLOSE_FAIL);
        return DISK_UNMOUNT_CLOSE_FAIL;
    }
    fs->disk = NULL;
    fs->fat = NULL;
    fs->root_dir = NULL;
    fs->fd = -1;
    fs->mounted = 0;
    return 1;
}