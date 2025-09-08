#include <string.h>
#include "shell.h"
#include "FS_info.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Extern_fs.h"
#include "DirHandle.h"
#include "FileHandle.h"
#include "Colors.h"
char* commands[] = {"FORMAT","MOUNT","UNMOUNT",
"MKDIR","CD","TOUCH","CAT","LS","APPEND","RM","HELP","CLOSE"};
int (*cmd_pointer[])(char**) = {
    &shell_format,
    &shell_mount,
    &shell_unmount,
    &shell_mkdir,
    &shell_cd,
    &shell_touch,
    &shell_cat,
    &shell_ls,
    &shell_append,
    &shell_rm,
    &shell_help,
    &shell_close,
};
void shell_loop(){
    char *line;
    char **args;
    int status;
    do{
        printf(GRN"$hell, digita comando: "RESET);
        line = shell_read();
        if(line == NULL){
            continue;
        }
        args = shell_split_line(line);
        status = shell_exec(args);
        free(line);
        free(args);
    }while(status);
}
char** shell_split_line(char* line){
    int size = MAX_ARGS;
    int pos = 0;
    char** tokens = (char**)malloc(sizeof(char*) * size);
    if(!tokens){
        printf("errore allocazione tokens\n");
        exit(-1);
    }
    char* token;
    token = strtok(line, " \n");
    while(token != NULL){
        tokens[pos] = token;
        pos++;
        if(pos >= size){
            size += MAX_ARGS;
            tokens = realloc(tokens, size*(sizeof(char*)));
            if(!tokens){
                printf("realloc fallita \n");
                exit(-1);
            }
        }
        token = strtok(NULL," \n");
    }
    tokens[pos] = NULL;
    return tokens;
}
int shell_exec(char** args){
    if(args[0] == NULL){
        printf( RED "Nessun comando inserito\n" RESET);
        return 1;
    }
    //for (int i = 0; args[i] != NULL; i++) {
    //printf("arg[%d] = '%s'\n", i, args[i]);
    //}

    for(int i=0;i<CMD_SIZE;++i){
        if(strcmp(args[0], commands[i]) == 0){
            return (*cmd_pointer[i])(args);
        }
    }
    puts(BLU "Comando non supportato, per maggiori informazoni : HELP" RESET);
    return 1;
}
char* shell_read(){
    char* line = (char*)malloc(MAX_ARG_LEN);
    if(!line){
        printf("shell_read fail\n");
        return NULL;
    }
    if(fgets(line,MAX_ARG_LEN,stdin) == NULL){
        free(line);
        return NULL;
    }
    return line;
}
int shell_format(char **args) {
    if(strcmp(args[0], "FORMAT") != 0 || args[1] == NULL){
        printf(BLU"Inserire : FORMAT <name>\n"RESET);
        return -1;
    }
    if (strcmp(args[0], "FORMAT") == 0) {
        if (args[1] == NULL) {
            printf(BLU"Inserire nome disco: FORMAT <name>, per semplicità aggiungere .fs al nome del disco \n"RESET);
            return -1;
        }
        char *fs_name = args[1];
        int fd = open(fs_name, O_RDONLY);
        if (fd >= 0) {
        printf(GRN"Disco '%s' esistente, formatto...\n",fs_name );
        close(fd);
        if (fs && fs->mounted) {
            if (disk_unmount(fs) < 0) {
                return -1;
            }
        }
        if (unlink(fs_name) < 0) {
            perror(RED"Errore nella rimozione del disco"RESET);
            return -1;
        }
    }
        if (!fs) {
            fs = fs_init();
            if (!fs) {
                return -1;
            }
        }
        if (disk_creat(fs_name, DISK_SIZE) < 0) {
            return -1;
        }
        printf(BLU"Disco formattato correttamente\n"RESET);
        return 1;
    }
    return -1;
}

int shell_mount(char **args) {
    if( strcmp(args[0],"MOUNT") != 0 || args[1] == NULL){
        printf(RED"Inserire: MOUNT <name>\n"RESET);
        return -1;
    }
    if( strcmp(args[0],"MOUNT") == 0){
        if(args[1] == NULL){
            printf(RED"Inserire nome disco: MOUNT <name>\n"RESET);
            return -1;
        }
        if(fs && fs->mounted){
            print_error(DISK_MOUNTED);
            return -1;
        }
        if(!fs){
            fs = fs_init();
            if(!fs){
                print_error(FS_NOTINIT);
                return -1;
            }
        }
        char* disk_name = args[1];
        int fd = open(disk_name,O_RDONLY);
        if(fd < 0){
            printf(RED "Disco non esistente\n" RESET);
            return -1;
        }
        if(close(fd) < 0 ){
            printf(BLU"Erorre chiusura in shellmount\n"RESET);
        }
        if(disk_mount(fs,disk_name) < 0){
            return -1;
        }
        return 1;
    }
    return -1;
}

int shell_unmount(char **args) {
    if(strcmp(args[0],"UNMOUNT") == 0 && args[1]!= NULL){
        printf(RED"Digitare solo UNMOUNT\n"RESET);
        return -1;
    }
    if(strcmp(args[0], "UNMOUNT") == 0){
       if(!fs || !fs->mounted){
        printf(RED"Nessun disco montato\n"RESET);
       }else{
        if(disk_unmount(fs)< 0){
            return -1;
        }else{
            printf(BLU"Disco smontato correttamente\n"RESET);
            return 1;
        }
       }
    }
    printf(RED"Errore shell unmount\n"RESET);
    return - 1;
}

int shell_mkdir(char **args) {
    if(strcmp(args[0],"MKDIR")== 0 && args[1] == NULL){
        printf(RED"Specificare nome/i del file: MKDIR <dir>"RESET);
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT, poi MOUNT\n"RESET);
        return -1;
    }
    int num_of_dirs = -1;
    for(int i = 0;;++i){
        if(args[i] != NULL) num_of_dirs+=1;
        else{
            break;
        }
    }
    DirHandle* dh;
    char* dirname;
    for(int i = 1; i <= num_of_dirs;++i){
        dirname = args[i];
        if(strstr(dirname,".") != NULL){
            printf(RED "Una cartella non può contenere un . nel nome\n" RESET);
            return -1;
        }
        dh = DirHandle_open(fs,dirname,PERM_CREAT|PERM_EXCL);
        if(!dh){
            printf(RED"Errore nella creazione di %s\n", dirname );
            return -1;
        }
        if(DirHandle_close(fs,dh)<0){
            return -1;
        }
        DirHandle_free(fs,dh);
    }
    if(num_of_dirs > 1){
        printf(BLU"Cartelle create correttamente\n"RESET);
    }else{
        printf(BLU"Cartella creata correttamente\n"RESET);
    }
    return 1;
}

int shell_cd(char **args) {
    if(strcmp(args[0],"CD") == 0 && args[1] == NULL){
        printf("Digitare CD <dest>\n");
        return -1;
    }
    if(!fs){
        printf("Prima FORMAT <disco>\n");
        return -1;
    }
    if(fs && !fs->mounted){
        printf("Prima MOUNT <disco>\n");
        return -1;
    }
    char* dir_name = args[1];
    if(Dir_Entry_change(fs,dir_name) < 0){
        printf("Errore nel cambio directory, esiste?\n");
        return -1;
    }
    return 1;
}

int shell_touch(char **args) {
    if(strcmp(args[0],"TOUCH")== 0 && args[1] == NULL){
        printf(RED"Specificare nome/i del file: TOUCH <file>"RESET);
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT, poi MOUNT <disk>\n"RESET);
        return -1;
    }
    int num_of_files = -1;
    for(int i = 0;;++i){
        if(args[i] != NULL) num_of_files+=1;
        else{
            break;
        }
    }
    //printf("Devo creare %d files\n", num_of_files);
    FileHandle* fh;
    for(int i = 1; i <= num_of_files;++i){
        fh = FileHandle_open(fs,args[i],PERM_CREAT|PERM_EXCL);
        if(!fh){
            printf(RED"Errore nella creazione di %s\n, il file già esiste", args[i] );
            return -1;
        }
        if(FileHandle_close(fs,fh)<0){
            return -1;
        }

    }
    if(num_of_files > 1){
        printf(BLU "Files creati correttamente\n"RESET);
    }else{
        printf(BLU"File creato correttamente\n"RESET);
    }
    return 1;
}

int shell_cat(char **args) {
    if(strcmp(args[0], "CAT") == 0 && args[1] == NULL){
        printf(RED"Inserire il nome del file da stampare\n"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT e MOUNT\n"RESET);
        return -1;
    }
    char* filename = args[1];
    Dir_Entry* target = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(!target){
        printf(RED"Nessun file trovato\n"RESET);
        return -1;
    }else{
        if(target->is_dir == 0){
            printf(RED"Selezionare un file, non una directory\n"RESET);
            return -1;
        }else{
            size_t len = target->file_size;
            char to_print[len+1];
            FileHandle* fh = FileHandle_open(fs,filename,PERM_READ);
            if(FileHandle_read(fs,fh,to_print,len) < 0){
                printf(RED"Errore lettura da shell\n"RESET);
                return -1;
            }else{
                to_print[len] = '\0';
                fwrite(to_print,1,len,stdout);
                printf("\n");
            }
            return 1;
        }
    }
    return 1;
}

int shell_ls(char **args) {
    if(strcmp(args[0],"LS") != 0){
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima è necessario creare o fare la mount del disco\n"RESET);
        return -1;
    }
    if(args[1] == NULL){
        printf("------------------------------------------\n");
        Dir_Entry_curr_list(fs);
    }else{
        char* to_list = args[1];
        Dir_Entry* target = Dir_Entry_find_name(fs,to_list,fs->curr_dir);
        if(!target){
            printf(RED"Dir non esistente"RESET);
        }else{
            Dir_Entry_list(fs,target->first_block);
        }
    }
    printf("------------------------------------------\n");
    return 1;
}

int shell_append(char **args) {
    if(strcmp(args[0],"APPEND") == 0 && args[1] == NULL){
        printf(RED"Occore specificare il file\n"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT e MOUNT\n"RESET);
        return -1;
    }
    char* filename = args[1];
    Dir_Entry* target = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(target){
        if(target->is_dir == 0){
            printf(RED"Non è possibile usare append su una cartella\n"RESET);
            return -1;
        }else{
            FileHandle* fh;
            char to_append[256] = {0};
            strcpy(to_append,args[2]);
            strcat(to_append," ");
            for(int i = 3;args[i] != NULL;i++){
                strcat(to_append,args[i]);
                strcat(to_append," ");
            }
            char* pos = to_append;
            while((pos = strstr(pos,"\\n")) != NULL){
                *pos = '\n';
                memmove(pos+1,pos+2,strlen(pos+2) + 1);
                pos++;
            }
            if(target->file_size == 0){
                fh = FileHandle_open(fs,filename,PERM_WRITE);
                if(!fh) return -1;
                if(FileHandle_write(fs,fh,to_append,strlen(to_append)) < 0){
                    printf("problemi in scrittura da shell\n");
                    return -1;
                }
            }else{
                fh = FileHandle_open(fs,filename,PERM_APPEND|PERM_WRITE);
                if(!fh) return -1;
                if(FileHandle_write(fs,fh,to_append,strlen(to_append)) < 0){
                    printf("problemi in scrittura da shell\n");
                    return -1;
                }
            }
            if(FileHandle_close(fs,fh)<0) return -1;
            return 1;
        }
    }else if(!target){
        printf(RED"Nessun file corrispondente a questo nome\n"RESET);
        return -1;
    }
    return -1;
}


int shell_rm(char **args) {
    if (strcmp(args[0], "RM") == 0 && args[1] == NULL) {
        printf(RED"Specificare almeno un file/cartella da rimuovere\n"RESET);
        return -1;
    }
    if (!fs || !fs->mounted) {
        printf(RED"Prima FORMAT, poi MOUNT\n"RESET);
        return -1;
    }    
    int rf = 0;
    int start = 1;
    if (args[1] != NULL && strcmp(args[1], "-RF") == 0) {
        rf = 1;
        start = 2;
        if (args[2] == NULL) {
            printf(RED"Specificare almeno un file/cartella\n"RESET);
            return -1;
        }
    }        
    int num_of_handles = 0;
    for (int i = start; args[i] != NULL; ++i) {
        num_of_handles++;
    }
    Dir_Entry* de;
    int ans;
    char* handle_name;
    for (int i = 0; i < num_of_handles; ++i) {
        handle_name = args[start + i];  
        de = Dir_Entry_find_name(fs, handle_name, fs->curr_dir);
        if (!de) {
            printf(RED"Non esiste %s\n", handle_name);
            continue;
        }
        if (de->is_dir == 0) { 
            if (rf) {
                printf(RED"Attenzione: eliminare una cartella cancellerà anche il suo contenuto, procedere ugualmente? [Y/n]\n"RESET);
                ans = getchar();
                while (getchar() != '\n');
                
                if (ans == 'Y' || ans == 'y') {
                    if (DirHandle_delete_force(fs, handle_name) < 0) {
                        printf("Errore in delete_force per %s\n", handle_name);
                        return -1;
                    }
                    printf(BLU"Directory %s eliminata\n", handle_name);
                } else {
                    printf(BLU"Eliminazione di %s annullata\n", handle_name);
                    continue;
                }
            } else {
                if (DirHandle_delete(fs, handle_name) < 0) {
                    printf(RED"Errore nell'eliminazione della directory %s\n", handle_name);
                    return -1;
                }
                printf(BLU"Directory %s eliminata\n", handle_name);
            }
        } 
        else { 
            if (FileHandle_delete(fs, handle_name) < 0) {
                printf(RED"Errore nell'eliminazione del file %s\n", handle_name);
                return -1;
            }
            printf(BLU"File %s eliminato\n", handle_name);
        }
    }
    return 1;
}

int shell_help(char **args) {
    printf(BLU"Comandi disponibili:\n"RESET);
    printf( "FORMAT <disco>\nMOUNT <disco>\nUNMOUNT \nMKDIR <dir> \nCD <dest> \nTOUCH <file> \nCAT <file> \nLS / LS <dir> \nAPPEND <file> <testo> \nRM <file> \nHELP\nCLOSE\n");
    return 1;
}
int shell_close(char **args) {
    if(strcmp(args[0],"CLOSE") == 0){
        if(!fs){
            printf("Nessun fs da chiudere\n");
            return 0;
        }
        if(fs->mounted){
            if(disk_unmount(fs) < 0){
                return -1;
            }
        }
        fs_free(&fs);
        fs = NULL;
        printf(BLU "Chiusura effettuata correttamente\n" RESET);
        return 0;
    }
    printf("shell close error\n");
    return -1;
}
