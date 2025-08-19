#pragma once
#include <stdio.h>
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
#define FS_ALLOC_FAIL -13
//Errore per cartella piena--find_free_dir_entry
#define FULL_DIR -14
//Errore per fat piena
#define FULL_FAT -15
//errrore nome troppo lungo
#define LONG_NAME -16
#define FH_ALLOC_FAIL -17
#define FILE_DUPLICATE -18
#define FH_FREE_FAIL -19
#define FH_NOTOPEN -20
#define NO_FREE_ENTRY -21
#define DIR_DUPLICATE -22
#define DH_ALLOC_FAIL -23
#define DH_NOTOPEN -24
#define DH_FREE_FAIL -25
#define DH_NOTINIT -32
#define FS_NOTINIT -26
#define INVALID_BLOCK -27
#define W_PERM -28
#define NO_WRITE -29
#define R_PERM -30
#define BAD_READ -31
#define FH_NOTINIT -33
#define NO_OFFSET -34
#define FILE_NOT_FOUND -35
#define FILE_ALR_OPEN -36
#define FH_OPEN_FAIL -37
#define DH_DUPLICATE -38
void print_error(int);