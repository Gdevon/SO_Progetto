#include <stdio.h>
#include <string.h>
#include "Errors.h"
#include "FileHandle.h"
#include "DIR_Entry.h"
#include "FS_info.h"
#include "ListHandle.h"

FileHandle* FileHandle_create(FileSystem* fs, char* filename){
    if(fs->mounted == 0){
        print_error(DISK_UNMOUNTED);
        return NULL;
    }
    if(check_filename(filename) < 0){
        print_error(LONG_NAME);
        return NULL;
    }
    if(check_duplicates(fs,filename) < 0){
        print_error(FILE_DUPLICATE);
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
    int type = is_dir(filename);
    Dir_Entry_create(free_entry,filename,free_fat,type);
    FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
    if(!fh){
        print_error(FH_ALLOC_FAIL);
        return NULL;
    }
    fh->dir = free_entry;
    fh->byte_offset = 0;
    fh->open = 1;
    fh->permission = PERM_WRITE;
    Handle_Item* item = (Handle_Item*)malloc(sizeof(Handle_Item));
    if(!item){
        free(fh);
        print_error(FS_ALLOC_FAIL);
        return NULL;
    }
    Handle_Item_create(item,fh,FILE_HANDLE);
    List_insert(&fs->handles,NULL,&item->h);
    return fh;
}


void FileHandle_free(FileSystem* fs, FileHandle *fh){
    if(!fs || !fh){
        print_error(FH_FREE_FAIL);
    }if(!fh->open){
        print_error(FH_NOTOPEN);
    }ListItem* current = fs->handles.first;
    while(current){
        Handle_Item *h_item = (Handle_Item*)current;
        if(h_item->handle == fh){
            List_detach(&fs->handles,current);
            ListItem_destroy(current);
            return;
        }
        current = current->next;
    }
}
void FileHandle_print(FileHandle* fh){
    if(fh->open == 1){
        printf("INFORMAZIONI FILEHANDLE %s\n", fh->dir->filename);
        printf("Byte offset: %u\n", fh->byte_offset);
        printf("Open : %u\n", fh->open);
        printf("Permission: %u\n", fh->permission);
        printf("----------------\n");
        Dir_Entry_print(fh->dir);
    }
    else{
        printf("FileHandle non aperto\n");
    }
}
int is_dir(char* filename){
    if(strstr(filename,".")) return 1; // è un file se contiene .
    else return 0; //cartella se non contiene .
}
int check_filename(char* filename){
    if(strlen(filename) > 14){
        return -1;
    }else{
        return 1;
    }
}
int check_duplicates(FileSystem* fs,char* filename){
    for(int i = 0;i<ROOT_DIR_BLOCKS*ENTRIES_PER_BLOCK;++i){
        if(fs->root_dir[i].filename[0] != '\0'){
            if(strcmp(fs->root_dir[i].filename,filename) == 0){
                return -1;
            }
        }
    }
    return 1;
}