#include "Errors.h"
void print_error(int err){
    switch (err) {
        case FULL_DIR:
            printf("Directory piena, impossibile trovare una entry libera\n");
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
        case DISK_UNMOUNTED:
            printf("Errore nell'unmounting del disco\n");
            break;
        case FS_ALLOC_FAIL:
            printf("Errore nell'allocazione del fs\n");
            break;
        case LONG_NAME:
            printf("Errore: nome file troppo lungo\n");
            break;
        case FH_ALLOC_FAIL:
            printf("Errore allocazione FileHandle\n");
            break;
        case FILE_DUPLICATE:
            printf("Errore: è già presente un file con questo nome\n");
            break;
        case NO_FREE_ENTRY:
            printf("Non ho trovato Dir_Entry libere\n");
            break;
        case DIR_DUPLICATE: 
            printf(" E' già presente una cartella con questo nome\n");
            break;
        case DH_ALLOC_FAIL:
            printf("Dh non allocata correttamente\n");
            break;
        case DH_NOTOPEN:
            printf("Dh non è aperta\n");
            break;
        case FS_NOTINIT:
            printf("Fs non inizializzato");
            break;
        case INVALID_BLOCK:
            printf("Numero del blocco > blocchi totali\n");
            break;
        case W_PERM:
            printf("Permessi di scrittura non abilitati");
            break;
        default:
            printf("Errore sconosciuto: %d\n", err);
            break;
    }
}