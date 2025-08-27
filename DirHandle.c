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
        printf("Non puoi creare senza un nome\n");
        return NULL;
    }
    if(strlen(dirname) > 30){
        print_error(LONG_NAME);
        return NULL;
    }
    Dir_Entry* target = Dir_Entry_find_name(fs,dirname,0);
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
        Dir_Entry* free_entry = Dir_Entry_find_free(fs,0);
        if(!free_entry){
            print_error(FULL_DIR);
            return NULL;
        }
        uint16_t free_fat = FAT_find_free_block(fs);
        if (free_fat == FAT_BLOCK_END) {
            print_error(FULL_FAT);
            return NULL;
        }
        fs->fat[free_fat] = FAT_BLOCK_END;

        int type = is_dir(dirname);
        Dir_Entry_create(free_entry, dirname, free_fat, type);
        target = free_entry;
    }
    DirHandle* dh = (DirHandle*)malloc(sizeof(DirHandle));
    if (!dh) {
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    dh->entries = NULL;
    dh->num_entries = 0;
    dh->position = 0;
    dh->first_block = target->first_block;
    dh->open = 1;
    Handle_Item* item = (Handle_Item*)malloc(sizeof(Handle_Item));
    if (!item) {
        free(dh);
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    Handle_Item_create(item, dh, DIR_HANDLE);
    List_insert(&fs->handles, NULL, &item->h);

    printf("Directory aperta con successo\n");
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
    if(dh->entries){
        free(dh->entries);
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
        print_error(DH_NOTINIT);//devo cambiare errore
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if(strlen(dirname) > 30){
        print_error(LONG_NAME);
        return -1;
    }
    Dir_Entry* target = Dir_Entry_find_name(fs,dirname,0);
    if(!target){
        print_error(DIR_NOT_FOUND);
        return -1;
    }
    if(target->is_dir != 0){
        print_error(NOT_A_DIR);
        return -1;
    }
    printf("debug delete 1\n");
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
    printf("debug delete 2\n");
    Dir_Entry* dir_entries;
    int max_entries;
    if(target->first_block == 0){
        dir_entries = fs->root_dir;
        max_entries = ROOT_DIR_BLOCKS*ENTRIES_PER_BLOCK;
    }
    else{
        uint32_t offset = (target->first_block - DATA_START_BLOCK)*BLOCK_SIZE;
        dir_entries = (Dir_Entry*)(fs->data + offset);
        printf("debug delete 3\n");
        max_entries = ENTRIES_PER_BLOCK;
    }
    for(int i = 0;i<max_entries;++i){
       if(&dir_entries[i] != target && dir_entries[i].filename[0] != '\0'){
        print_error(DIR_NOT_EMPTY);
        return -1;
       }
    }
    if(target->first_block >= DATA_START_BLOCK){
        fs->fat[target->first_block] = FAT_FREE_BLOCK;
    }
    memset(target,0,sizeof(Dir_Entry));
    printf("Directory %s eliminata\n", dirname);
    return 1;
}