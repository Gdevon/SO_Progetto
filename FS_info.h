#pragma once
#define SIZE_BLOCK 512                //512byte per ogni blocco
#define SIZE_DISK (32 * 1024 * 1024)  //32mb didisco
#define TOTAL_BLOCKS (SIZE_DISK / SIZE_BLOCK) 

//divisione del fs |reserved|fat|root|data|
#define BOOT_SECTOR_BLOCKS 1           //ricorda: inutile nel mio caso essendo già tutto caricato
#define FAT_BLOCKS 256                //ogni blocco di fat puo contenere fino a 256 indici di blocchi di file: essendo un blocco da 512 bytes e visto che un indirizzo richiede 2 bytes al più per essere memorizzato
#define ROOT_DIR_BLOCKS 32            
#define DATA_START_BLOCK (BOOT_SECTOR_BLOCKS + FAT_BLOCKS + ROOT_DIR_BLOCKS) 
#define DATA_BLOCKS (TOTAL_BLOCKS - DATA_START_BLOCK)                        
