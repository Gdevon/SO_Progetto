#include <string.h>
#include "DirHandle.h"
#include "Errors.h"
#include "FS_info.h"
#include "DIR_Entry.h"
#include "FAT_info.h"
#include "ListHandle.h"
DirHandle* DirHandle_create(FileSystem* fs,char* dirname){
    if(fs->mounted == 0){
        print_error(DISK_UNMOUNTED);
        return NULL;
    }
    if(strlen(dirname) > 14){
        print_error(LONG_NAME);
        return NULL;
    }
    if(check_duplicates(fs,dirname) < 0){
        print_error(DIR_DUPLICATE);
        return NULL;
    }
    Dir_Entry* free_entry = Dir_Entry_find_free(fs);
    if(!free_entry){
        print_error(FULL_DIR);
        return NULL;
    }
    uint16_t free_fat = FAT_find_free_block(fs);
    if(free_fat == FAT_BLOCK_END){
        print_error(FULL_FAT);
        return NULL;
    }
    fs->fat[free_fat] = FAT_BLOCK_END;
    int type = is_dir(dirname);
    Dir_Entry_create(free_entry,dirname,free_fat,type);
    DirHandle* dh = (DirHandle*)malloc(sizeof(DirHandle));
    if(!dh){
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    dh->entries = NULL;
    dh->num_entries = 0;
    dh->position = 0;
    dh->first_block = free_fat;
    dh->open = 1;
    Handle_Item* item = (Handle_Item*)malloc(sizeof(Handle_Item));
    if(!item){
        free(dh);
        print_error(DH_ALLOC_FAIL);
        return NULL;
    }
    Handle_Item_create(item,dh,DIR_HANDLE);
    List_insert(&fs->handles,NULL,&item->h);
    return dh;
}
void DirHandle_free(FileSystem* fs, DirHandle* dh){
    if(!fs || !dh){
        print_error(DH_FREE_FAIL);
        return;
    }
    if(!dh->open){
        print_error(DH_NOTOPEN);
        return;
    }
    ListItem* curr = fs->handles.first;
    while(curr){
        Handle_Item* it = (Handle_Item*)curr;
        if(it->handle == dh && it->type == DIR_HANDLE){
            List_detach(&fs->handles,curr);
            if(dh->entries){
                free(dh->entries);
            }
            free(dh);
            ListItem_destroy(curr);
            return;
        }
        curr = curr->next;
    }
}
