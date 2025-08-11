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
    if(strlen(filename) > 14){
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
        return;
    }if(!fh->open){
        print_error(FH_NOTOPEN);
        return;
    }ListItem* current = fs->handles.first;
    while(current){
        Handle_Item *h_item = (Handle_Item*)current;
        if(h_item->handle == fh && h_item->type == FILE_HANDLE){
            List_detach(&fs->handles,current);
            ListItem_destroy(current);
            return;
        }
        current = current->next;
    }
}
/*void FileHandle_print(FileHandle* fh){
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
} DA RIFARE */
int is_dir(char* filename){
    if(strstr(filename,".")) return 1; // è un file se contiene .
    else return 0; //cartella se non contiene .
}

int FileHandle_write(FileSystem* fs, FileHandle* fh, char* buffer, size_t size_to_write) {
    if (!fs) {
        print_error(FS_NOTINIT);
        return -1;
    }
    if (!fs->mounted) {
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    if (!fh) {
        print_error(FH_ALLOC_FAIL);
        return -1;
    }
    if (!fh->open) {
        print_error(FH_NOTOPEN);
        return -1;
    }
    if (!(fh->permission & PERM_WRITE)) {
        print_error(W_PERM);
        return -1;
    }
    if(!buffer || size_to_write == 0){
        return 0;
    }

    size_t curr_offset = fh->byte_offset;
    uint16_t curr_block = fh->dir->first_block;
    size_t written_bytes = 0;
    if(curr_block == 0 || curr_block == FAT_BLOCK_END || curr_block == FAT_FREE_BLOCK){
        curr_block = FAT_find_free_block(fs);
        if(curr_block == (uint16_t) -1){
            print_error(FULL_FAT);
            return -1;
        }
        fh->dir->first_block = curr_block;
        fs->fat[curr_block] = FAT_BLOCK_END;
        curr_offset = 0;
        fh->byte_offset = 0;
    }
    uint16_t block_idx = curr_offset / BLOCK_SIZE;
    size_t block_offset = curr_offset % BLOCK_SIZE;

    for (size_t i = 0; i < block_idx ; ++i) {
        if(curr_block == FAT_BLOCK_END){
            print_error(INVALID_BLOCK);
            return -1;
        }
        uint16_t next_block = fs->fat[curr_block];
        if(next_block == FAT_BAD || next_block == 0){
            print_error(INVALID_BLOCK);
            return -1;
        }
        curr_block = next_block;
    }

    while (written_bytes < size_to_write) {
        size_t bytes_to_write = BLOCK_SIZE - block_offset;
        if (bytes_to_write > size_to_write - written_bytes) {
            bytes_to_write = size_to_write - written_bytes;
        } 
        char data_block[BLOCK_SIZE];
        if (block_offset != 0 || bytes_to_write < BLOCK_SIZE) {
            if (fs_read_block(fs, curr_block, data_block) < 0) {
                print_error(INVALID_BLOCK);
                return written_bytes;
            }
        }
        memcpy(data_block + block_offset, buffer + written_bytes, bytes_to_write);
        if (fs_write_block(fs, curr_block, data_block) < 0) {
            print_error(INVALID_BLOCK);
            return written_bytes;
        }

        written_bytes += bytes_to_write;
        curr_offset += bytes_to_write;
        fh->byte_offset = curr_offset;
        block_offset = 0;

        if (written_bytes < size_to_write) {
            uint16_t next_block = fs->fat[curr_block];
            if (next_block == FAT_BLOCK_END) {
                next_block = FAT_find_free_block(fs);
                if(next_block == FAT_BAD){
                    print_error(FULL_FAT);
                    return written_bytes;
                }
                fs->fat[curr_block] = next_block;
                fs->fat[next_block] = FAT_BLOCK_END;
            }else if(next_block == FAT_BAD){
                print_error(INVALID_BLOCK);
                return written_bytes;
            }
            curr_block = next_block;
        }
    }
    if (fh->byte_offset > fh->dir->file_size) {
        fh->dir->file_size = fh->byte_offset;
    }
    return written_bytes;
}