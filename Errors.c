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
            printf("Fs non inizializzato\n");
            break;
        case INVALID_BLOCK:
            printf("Numero del blocco > blocchi totali\n");
            break;
        case W_PERM:
            printf("Permessi di scrittura non abilitati\n");
            break;
        case NO_WRITE:  
            printf("Scrittura di 0 bytes o su buffer non allocato\n");
            break;
        case R_PERM:
            printf("Permessi di lettura non abilitati\n");
            break;
        case BAD_READ:  
            printf("Problemi in lettura\n");
            break;
        case FH_NOTOPEN:
            printf("Fh non aperto\n");
            break;
        case FH_NOTINIT:    
            printf("Il fh non esiste\n");
            break;
        case NO_OFFSET: 
            printf("Errore con l'offset di FileHandle seek\n");
            break;
        case FILE_NOT_FOUND:
            printf("File non trovato\n");
            break;
        case FILE_ALR_OPEN:
            printf("Non si può eleiminare un file aperto\n");
            break;
        case DIR_NOT_FOUND:
            printf("Impossibile trovare la dir\n");
            break;
        case NOT_A_DIR:
            printf("Not a dir\n");
            break;
        case ROOT_AREA:
            printf("Non è possibile eseguire operazioni sulla root");
            break;
        case DIR_NOT_EMPTY:
            printf("Non è possibile eliminare una dir non vuota\n");
            break;
        default:
            printf("Errore sconosciuto: %d\n", err);
            break;
    }
}