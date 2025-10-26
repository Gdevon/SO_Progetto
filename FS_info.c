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
#include "Colors.h"
int disk_creat(char* disk_name, uint32_t size){ 
    if(size != DISK_SIZE){
        print_error(DISK_SIZE); 
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
    uint8_t* disk = (uint8_t*)mmap(NULL,DISK_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0); 
    if(disk == MAP_FAILED){
        print_error(DISK_CREAT_MMAP_FAIL);
        if(close(fd) < 0 ){
            print_error(DISK_CREAT_CLOSE_FAIL);
            return DISK_CREAT_CLOSE_FAIL;
        }
        return DISK_CREAT_MMAP_FAIL;
    }
    memset(disk,0,DISK_SIZE);
    uint16_t* fat = (uint16_t*)(disk + (BLOCK_SIZE*BOOT_SECTOR_BLOCKS)); 
    for(int i = 0; i < FAT_ENTRIES; ++i){
        fat[i] = FAT_FREE_BLOCK;
    }
    Dir_Entry* root_dir = (Dir_Entry*)(disk+ (BLOCK_SIZE)*(BOOT_SECTOR_BLOCKS+FAT_BLOCKS)); 
    memset(root_dir,0,BLOCK_SIZE*ROOT_DIR_BLOCKS);
    uint8_t* data = (uint8_t*)(disk+(BLOCK_SIZE*DATA_START_BLOCK));
    memset(data,0,BLOCK_SIZE*DATA_BLOCKS);
    if(munmap(disk,DISK_SIZE) < 0){
        print_error(DISK_CREAT_MUNMAP_FAIL);
        return DISK_CREAT_MUNMAP_FAIL;
    }
    if(close(fd)<0){
        print_error(DISK_CREAT_CLOSE_FAIL);
        return DISK_CREAT_CLOSE_FAIL;
    }
    //printf("Disk_creat ha avuto successo\n");
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
    fs->fat = (uint16_t*)(disk+(BLOCK_SIZE*BOOT_SECTOR_BLOCKS));
    fs->root_dir = (Dir_Entry*)(disk+BLOCK_SIZE*(FAT_BLOCKS+BOOT_SECTOR_BLOCKS));
    fs->data = (uint8_t*)(disk+ (BLOCK_SIZE*DATA_START_BLOCK));
    fs->mounted = 1;
    fs->curr_dir = ROOT_DIR_START_BLOCK;
    List_init(&fs->handles);
    printf(BLU "Disco montato correttamente\n" RESET);
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
    //printf("Disk_Unmount ha avuto successo\n");
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
        fs->curr_dir = ROOT_DIR_START_BLOCK;
       // printf("FS inizializzato\n");
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
       // printf("free fs\n");
    }
}
int fs_write_block(FileSystem* fs, uint16_t block_id, void* buffer){ 
    if(!fs){
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if(block_id >= TOTAL_BLOCKS){
        print_error(INVALID_BLOCK);
        return -1;
    }
    uint32_t offset = block_id*BLOCK_SIZE;
    memcpy(fs->disk+offset,buffer,BLOCK_SIZE);
   // printf("Blocco scritto\n");
    return 1;
}
int fs_read_block(FileSystem* fs, uint16_t block_id,void* buffer){
    if(!fs){
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if(block_id >= TOTAL_BLOCKS){
        print_error(INVALID_BLOCK);
        return -1;
    }
    uint32_t offset = block_id * BLOCK_SIZE;
    memcpy(buffer, fs->disk+offset,BLOCK_SIZE);
   // printf("Write_block completata\n");
    return 1;
}
size_t fs_explore_block(FileSystem* fs, uint16_t block){
    if(!fs){
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if(block >= TOTAL_BLOCKS){
        print_error(INVALID_BLOCK);
        return -1;
    }
    size_t off = 0;
    while(off < BLOCK_SIZE){
        if(fs->disk[block*BLOCK_SIZE+off] == '\0'){
            return off;
        }
        off++;
    }
    return off;
}

