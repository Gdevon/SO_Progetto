#include <stdio.h>
#include <string.h>
#include "Errors.h"
#include "FileHandle.h"
#include "DIR_Entry.h"
#include "FS_info.h"
#include "ListHandle.h"
#include "Colors.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

FileHandle* FileHandle_open(FileSystem* fs, char* filename,Permission perm){
    if (!fs) {
        print_error(FS_NOTINIT);
        return NULL;
    }
    if (!fs->mounted) {
        print_error(DISK_UNMOUNTED);
        return NULL;
    }
    if (strlen(filename) > 46) {
        print_error(LONG_NAME);
        return NULL;
    }
    Dir_Entry* target = Dir_Entry_find_name(fs, filename,fs->curr_dir);
    if (target) {
        if (target->is_dir == 0) {
            print_error(NOT_A_FILE);
            return NULL;
        }

        if ((perm & PERM_CREAT) && (perm & PERM_EXCL)) {
            print_error(FILE_DUPLICATE);
            return NULL;
        }
        if ((perm & PERM_READ) && !(target->perms & PERM_READ)) {
            print_error(R_PERM);
        return NULL;
        }
        if ((perm & PERM_WRITE) && !(target->perms & PERM_WRITE)) {
        print_error(W_PERM);
        return NULL;
    }
    } else {
        if (!(perm & PERM_CREAT)) {
            //printf("debug");
            print_error(FILE_NOT_FOUND);
            return NULL;
        }
        Dir_Entry* free_entry = Dir_Entry_find_free(fs,fs->curr_dir);
        if (!free_entry) {
            return NULL;
        }
        uint16_t free_block = FAT_find_free_block(fs);
        if (free_block == FAT_BLOCK_END) {
            print_error(FULL_FAT);
            return NULL;
        }
        uint16_t fat_idx = free_block - DATA_START_BLOCK;
        fs->fat[fat_idx] = FAT_BLOCK_END;
        Dir_Entry_create(fs,free_entry, filename, free_block, 1);
        target = free_entry;
    }
    update_access_date(target);
    FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));
    if (!fh) {
        print_error(FH_ALLOC_FAIL);
        return NULL;
    }
    fh->dir = target;
    if(perm & PERM_WRITE){
        fh->byte_offset = target->file_size;
    }else{
    fh->byte_offset = 0;
    }
    fh->open = 1;
    fh->permission = perm;
    Handle_Item* item = (Handle_Item*)malloc(sizeof(Handle_Item));
    if (!item) {
        free(fh);
        print_error(FH_ALLOC_FAIL);
        return NULL;
    }
    Handle_Item_create(item, fh, FILE_HANDLE);
    List_insert(&fs->handles, NULL, &item->h);
    //printf("File %s aperto con successo\n", fh->dir->filename);
    return fh;
}
int FileHandle_close(FileSystem* fs, FileHandle *fh){
    if(!fs || !fh){
        print_error(FH_FREE_FAIL);
        return -1 ;
    }if(!fh->open){
        print_error(FH_NOTOPEN);
        return -1;
    }
    ListItem* current = fs->handles.first;
    while(current){
        Handle_Item *h_item = (Handle_Item*)current;
        if(h_item->handle == fh && h_item->type == FILE_HANDLE){
            fh->open = 0;
            List_detach(&fs->handles,current);
            free(h_item);
            //printf("Close di %s effettuata\n", fh->dir->filename);
            break;
        }
        current = current->next;
    }
    free(fh);
    return 1;
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
        print_error(NO_WRITE);
        return -1;
    }

    size_t curr_offset = fh->byte_offset;
    uint16_t curr_block = fh->dir->first_block;
    size_t written_bytes = 0;
    if(curr_block == FAT_FREE_BLOCK || curr_block == FAT_BLOCK_END || curr_block == FAT_FREE_BLOCK){
        curr_block = FAT_find_free_block(fs);
        if(curr_block == (uint16_t) -1){
            print_error(FULL_FAT);
            return -1;
        }
        fh->dir->first_block = curr_block;
        fs->fat[curr_block-DATA_START_BLOCK] = FAT_BLOCK_END;
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
        uint16_t next_block = fs->fat[curr_block-DATA_START_BLOCK];
        if(next_block == FAT_BAD){
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
            uint16_t next_block = fs->fat[curr_block-DATA_START_BLOCK];
            if (next_block == FAT_BLOCK_END) {
                next_block = FAT_find_free_block(fs);
                if(next_block == FAT_BAD){
                    print_error(FULL_FAT);
                    return written_bytes;
                }
                fs->fat[curr_block - DATA_START_BLOCK] = next_block;
                fs->fat[next_block-DATA_START_BLOCK] = FAT_BLOCK_END;
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
    update_modify_time(fh->dir);
    //printf("FileHandle_write completata\n");
    return written_bytes;
}
int FileHandle_read(FileSystem* fs, FileHandle* fh, char* buffer,size_t size_to_read){
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
    if (!(fh->permission & PERM_READ)) {
        print_error(R_PERM);
        return -1;
    }
    if(!buffer || size_to_read == 0){
        return 0;
    }
    size_t curr_offset = fh->byte_offset;
    uint16_t curr_block = fh->dir->first_block;
    size_t read_bytes = 0;
    if(curr_block == FAT_BLOCK_END){
        return 0;
    }
    if(curr_block == FAT_BAD){
        print_error(BAD_READ);
        return -1;
    }
    size_t bytes_to_read = fh->dir->file_size - curr_offset;
    if(bytes_to_read == 0) return 0;
    if(size_to_read > bytes_to_read){
        size_to_read = bytes_to_read;
    }
    uint16_t block_idx = curr_offset / BLOCK_SIZE;
    size_t block_offset = curr_offset % BLOCK_SIZE;
    for(size_t i = 0;i < block_idx; ++i){
        if(curr_block == FAT_BLOCK_END){
            print_error(INVALID_BLOCK);
            return -1;
        }
        uint16_t next_block = fs->fat[curr_block-DATA_START_BLOCK];
        if(next_block == FAT_BAD){
            print_error(INVALID_BLOCK);
            return -1;
        }
        curr_block = next_block;
    }
    while(read_bytes < size_to_read){
        size_t bytes_to_read_in_block = BLOCK_SIZE - block_offset;
        if(bytes_to_read_in_block > size_to_read - read_bytes){
            bytes_to_read_in_block = size_to_read - read_bytes;
        }
        char data_block[BLOCK_SIZE];
        if(fs_read_block(fs,curr_block,data_block) < 0 ){
            print_error(INVALID_BLOCK);
            return read_bytes;
        }
        memcpy(buffer+read_bytes, data_block+block_offset,bytes_to_read_in_block);
        read_bytes += bytes_to_read_in_block;
        curr_offset += bytes_to_read_in_block;
        fh->byte_offset = curr_offset;
        block_offset = 0;
        if(read_bytes < size_to_read){
            //if(curr_block < DATA_START_BLOCK || curr_block >= DATA_START_BLOCK + DATA_BLOCKS){
            //    print_error(INVALID_BLOCK);
            //    return read_bytes;
            //}
            uint16_t next_block = fs->fat[curr_block-DATA_START_BLOCK];
            if(next_block == FAT_BLOCK_END){
                return read_bytes;
            }
            if(next_block == FAT_BAD){
                print_error(INVALID_BLOCK);
                return read_bytes;
            }
            curr_block = next_block;
        }
    }
    update_access_date(fh->dir);
    return read_bytes;
}
int FileHandle_seek(FileHandle* fh, int offset, int where){
    if(!fh){
        print_error(FH_NOTINIT);
        return -1;
    }
    if(!fh->open){
        print_error(FH_NOTOPEN);
        return -1;
    }
    size_t pos;
    if(where == SEEK_SET){
        if(offset < 0){
            print_error(NO_OFFSET);
            return -1;
        }
        pos = offset;
    }else if(where == SEEK_CUR){
        if( (offset < 0) && (-offset) > fh->byte_offset){
            print_error(NO_OFFSET);
            return -1;
        }pos = fh->byte_offset + offset;
    }else if(where == SEEK_END){
        if(offset > 0){
            print_error(NO_OFFSET);
            return -1;
        }
        if((-offset) > fh->dir->file_size){
            print_error(NO_OFFSET);
            return -1;
        }
        pos = fh->dir->file_size + offset;
    }else{
        print_error(NO_OFFSET);
        return -1;
    }
    if(pos > fh->dir->file_size){
        print_error(NO_OFFSET);
        return -1;
    }
    fh->byte_offset = pos;
    return 1;
}
int FileHandle_tell(FileHandle* fh){
    if(!fh){
        print_error(FH_NOTINIT);
        return -1;
    }
    if(!fh->open){
        print_error(FH_NOTOPEN);
        return -1;
    }
    return fh->byte_offset;
}
int FileHandle_delete(FileSystem* fs, char* filename){
    if(!fs){
        print_error(FS_NOTINIT);
        return -1;
    }
    if(!fs->mounted){
        print_error(DISK_UNMOUNTED);
        return -1;
    }
    Dir_Entry* entry = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(!entry){
        print_error(FILE_NOT_FOUND);
        return -1;
    }
    if(entry->is_dir == 0){
        print_error(NOT_A_FILE);
        return -1;
    }
    ListItem* curr = fs->handles.first;
    while(curr){
        Handle_Item* item = (Handle_Item*) curr;
        if(item->type == FILE_HANDLE){
            FileHandle* fh = (FileHandle*) item->handle;
            if(fh->dir == entry && fh->open){
                print_error(FILE_ALR_OPEN);
                return -1;
            }
        }
        curr = curr->next;
    }

    if(FAT_free_chain(fs,entry->first_block)<0){
        printf("Errore filehandle delete lib fat\n");
        return -1;
    }
    memset(entry,0,sizeof(Dir_Entry));
    entry->first_block = FAT_FREE_BLOCK;
    //printf("file eliminato con successo\n");
    return 1;
}
int FileHandle_change_perm(FileHandle* fh, Permission perm){
    if(!fh){
        print_error(FH_NOTINIT);
        return -1;
    }
    if(fh->open == 0){
        print_error(FH_NOTOPEN);
        return -1;
    }
    fh->permission = perm;
    fh->dir->perms = perm;
    return 1;
}
void FileHandle_print_perm(FileHandle* fh) {
    if (!fh) {
        printf("FileHandle nullo\n");
        return;
    }
    printf("Permessi attivi per '%s': ", fh->dir->filename);
    if (fh->permission == PERM_NO) {
        printf("NESSUNO\n");
        return;
    }
    if (fh->permission & PERM_READ)  printf("PERM_READ ");
    if (fh->permission & PERM_WRITE) printf("PERM_WRITE ");
    if (fh->permission & PERM_CREAT) printf("PERM_CREAT ");
    if (fh->permission & PERM_EXCL)  printf("PERM_EXCL ");
    printf("\n");
}