#include <string.h>
#include "DirHandle.h"
#include "Errors.h"
#include "FS_info.h"
#include "DIR_Entry.h"
#include "FAT_info.h"
#include "ListHandle.h"

DirHandle* DirHandle_open(FileSystem* fs,char* dirname,Permission perm){
    if(!fs){
        print_error(FS_NOTINIT);
        return NULL;
    }
    if(fs->mounted == 0){
        print_error(DISK_UNMOUNTED);
        return NULL;
    }
    if(dirname[0] == '\0'){
        print_error(EMPTY_NAME);
        return NULL;
    }
    if(strlen(dirname) > 46){
        print_error(LONG_NAME);
        return NULL;
    }
    uint16_t parent_block = fs->curr_dir;
    Dir_Entry* target = Dir_Entry_find_name(fs,dirname,parent_block);
    if(target){
        if(target->is_dir == 1){
            print_error(NOT_A_DIR);
            return NULL;
        }
        if((perm & PERM_CREAT) && (perm & PERM_EXCL)){
            print_error(DH_DUPLICATE);
            return NULL;
        }
    }else{
        if(!(perm & PERM_CREAT)){
            print_error(DIR_NOT_FOUND);
            return NULL;
        }
        Dir_Entry* free_entry = Dir_Entry_find_free(fs,parent_block);
        if(!free_entry){
            print_error(FULL_DIR);
            return NULL;
        }
        uint16_t free_block = FAT_find_free_block(fs);
        if (free_block == FAT_BLOCK_END) {
            print_error(FULL_FAT);
            return NULL;
        }
        uint16_t fat_idx = free_block - DATA_START_BLOCK;
        fs->fat[fat_idx] = FAT_BLOCK_END;
        Dir_Entry_create(fs,free_entry, dirname, free_block, 0);
        target = free_entry;
    }
    DirHandle* dh = (DirHandle*)malloc(sizeof(DirHandle));
    if (!dh) {
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    dh->first_block = target->first_block;
    uint32_t block_offset = (dh->first_block - DATA_START_BLOCK) * BLOCK_SIZE;
    dh->entries = (Dir_Entry*)(fs->data+block_offset);
    dh->num_entries = ENTRIES_PER_BLOCK;
    dh->open = 1;
    dh->position = 0;
    Handle_Item* item = (Handle_Item*)malloc(sizeof(Handle_Item));
    if (!item) {
        free(dh);
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    Handle_Item_create(item, dh, DIR_HANDLE);
    List_insert(&fs->handles, NULL, &item->h);
    //printf("Directory aperta con successo\n");
    return dh;
}

int DirHandle_close(FileSystem* fs, DirHandle* dh){
    if(!fs || !dh){
        print_error(DH_FREE_FAIL);
        return -1;
    }
    if(!dh->open){
        print_error(DH_NOTOPEN);
        return -1;
    }
    ListItem* curr = fs->handles.first;
    while(curr){
        Handle_Item* it = (Handle_Item*)curr;
        if(it->handle == dh && it->type == DIR_HANDLE){
            dh->open = 0;
            List_detach(&fs->handles,curr);
            free(it);
            return 1;
        }
        curr = curr->next;
    }
    return -1;
}
void DirHandle_free(FileSystem* fs, DirHandle* dh){
    if(!dh){
        print_error(DH_NOTINIT);
        return;
    }
    if(dh->open){
        DirHandle_close(fs,dh);
    }
    free(dh);
}
void DirHandle_list(FileSystem* fs, DirHandle* dh){
    if (!fs) {
        print_error(FS_NOTINIT);
        return;
    }
    if (!dh) {
        print_error(DH_NOTINIT);
        return;
    }
    if (!dh->open) {
        print_error(DH_NOTOPEN);
        return;
    }
    if (fs->mounted == 0) {
        print_error(DISK_UNMOUNTED);
        return;
    }    
    Dir_Entry_list(fs, dh->first_block);
}
int DirHandle_delete(FileSystem* fs, char* dirname){
    if(!fs){
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!dirname){
        print_error(EMPTY_NAME);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if(strlen(dirname) > 46){
        print_error(LONG_NAME);
        return -1;
    }
    Dir_Entry* target = Dir_Entry_find_name(fs,dirname,fs->curr_dir);
    if(!target){
        print_error(DIR_NOT_FOUND);
        return -1;
    }
    if(target->is_dir == 1){
        print_error(NOT_A_DIR);
        return -1;
    }
    ListItem* curr = fs->handles.first;
    while(curr){
        Handle_Item* it = (Handle_Item*)curr;
        if(it->type == DIR_HANDLE){
            DirHandle* dh = (DirHandle*)it->handle;
            if(dh->first_block == target->first_block){
                print_error(DIR_IN_USE);
                return -1;
            }
        }
        curr = curr->next;
    }
    if(fs->curr_dir == target->first_block){
        print_error(DIR_IN_USE);
        return -1;
    }
    uint32_t offset = (target->first_block - DATA_START_BLOCK) * BLOCK_SIZE;
    Dir_Entry* dir_entries = (Dir_Entry*)(fs->data+offset);
    for (int i = 0; i < ENTRIES_PER_BLOCK; ++i) {
        if (dir_entries[i].filename[0] != '\0' &&
            strcmp(dir_entries[i].filename, ".") != 0 &&
            strcmp(dir_entries[i].filename, "..") != 0) {
            print_error(DIR_NOT_EMPTY);
            return -1;
        }
    }
    if(FAT_free_chain(fs,target->first_block) <0 ){
        printf("Errore nella liberazione della fat\n");
        return -1;
    }
    memset(target,0,sizeof(Dir_Entry));
    //printf("Eliminazione %s effettuata\n",dirname);
    return 1;
}
int DirHandle_delete_recursive(FileSystem* fs, uint16_t dir_block) {
    if (!fs || !fs->mounted) {
        print_error(FS_NOTINIT);
        return -1;
    }
    if (dir_block < DATA_START_BLOCK) {
        print_error(INVALID_BLOCK);
        return -1;
    }
    if (fs->curr_dir == dir_block) {
        print_error(DIR_IN_USE);
        return -1;
    }    
    ListItem* curr = fs->handles.first;
    while (curr) {
        Handle_Item* it = (Handle_Item*)curr;
        if (it->type == DIR_HANDLE) {
            DirHandle* dh = (DirHandle*)it->handle;
            if (dh->first_block == dir_block) {
                print_error(DIR_IN_USE);
                return -1;
            }
        }
        curr = curr->next;
    }
    uint32_t offset = (dir_block - DATA_START_BLOCK) * BLOCK_SIZE;
    Dir_Entry* dir_entries = (Dir_Entry*)(fs->data + offset);    
    for (int i = 0; i < ENTRIES_PER_BLOCK; ++i) {
        if (dir_entries[i].filename[0] != '\0' &&
            strcmp(dir_entries[i].filename, ".") != 0 &&
            strcmp(dir_entries[i].filename, "..") != 0) {
            if (dir_entries[i].is_dir == 0) {
                if (DirHandle_delete_recursive(fs, dir_entries[i].first_block) < 0) {
                    return -1;
                }
            } else {
                ListItem* file_curr = fs->handles.first;
                int file_is_open = 0;
                while(file_curr){
                    Handle_Item* file_item = (Handle_Item*) file_curr;
                    if(file_item->type == FILE_HANDLE){
                        FileHandle* fh = (FileHandle*) file_item->handle;
                        if(fh->dir == &dir_entries[i] && fh->open){
                            file_is_open = 1;
                            break;
                        }
                    }
                    file_curr = file_curr->next;
                }
                if(file_is_open){
                    print_error(FILE_ALR_OPEN);
                    return -1;
                }
                if(FAT_free_chain(fs, dir_entries[i].first_block) < 0){
                    return -1;
                }
            }
        }
    }
    for (int i = 0; i < ENTRIES_PER_BLOCK; ++i) {
        if (dir_entries[i].filename[0] != '\0' &&
            strcmp(dir_entries[i].filename, ".") != 0 &&
            strcmp(dir_entries[i].filename, "..") != 0) {
            memset(&dir_entries[i], 0, sizeof(Dir_Entry));
        }
    }    
    if (FAT_free_chain(fs, dir_block) < 0) {
        return -1;
    }
    return 1;
}
int DirHandle_delete_force(FileSystem* fs, char* dirname) {
    if (!fs) {
        print_error(FS_NOTINIT);
        return -1;
    }
    if (!dirname) {
        print_error(EMPTY_NAME);
        return -1;
    }
    if (!fs->mounted) {
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if (strlen(dirname) > 46) {
        print_error(LONG_NAME);
        return -1;
    }
    Dir_Entry* target = Dir_Entry_find_name(fs, dirname, fs->curr_dir);
    if (!target) {
        print_error(DIR_NOT_FOUND);
        return -1;
    }
    if (target->is_dir == 1) {
        print_error(NOT_A_DIR);
        return -1;
    }    
    if (DirHandle_delete_recursive(fs, target->first_block) < 0) {
        return -1;
    }    
    memset(target, 0, sizeof(Dir_Entry));
    return 1;
}