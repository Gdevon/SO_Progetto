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
    &shell_close
};
void shell_loop(){
    char *line;
    char **args;
    int status;
    do{
        printf("$hell, digita comando: ");
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
        printf("Nessun comando inserito\n");
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
    puts("Comando non supportato, per maggiori informazoni : HELP");
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
        printf("Inserire : FORMAT <name>\n");
        return -1;
    }
    if (strcmp(args[0], "FORMAT") == 0) {
        if (args[1] == NULL) {
            printf("Inserire nome disco: FORMAT <name> \n");
            return -1;
        }
        char *fs_name = args[1];
        int fd = open(fs_name, O_RDONLY);
        if (fd >= 0) {
        printf("Disco '%s' esistente, formatto...\n", fs_name);
        close(fd);
        if (fs && fs->mounted) {
            if (disk_unmount(fs) < 0) {
                return -1;
            }
        }
        if (unlink(fs_name) < 0) {
            perror("Errore nella rimozione del disco");
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
        printf("Disco formattato correttamente\n");
        return 1;
    }
    return -1;
}

int shell_mount(char **args) {
    if( strcmp(args[0],"MOUNT") != 0 || args[1] == NULL){
        printf("Inserire: MOUNT <name>\n");
        return -1;
    }
    if( strcmp(args[0],"MOUNT") == 0){
        if(args[1] == NULL){
            printf("Inserire nome disco: MOUNT <name>\n");
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
            printf("Disco non esistente\n");
            return -1;
        }
        if(close(fd) < 0 ){
            printf("Erorre chiusura in shellmount\n");
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
        printf("Digitare solo UNMOUNT\n");
        return -1;
    }
    if(strcmp(args[0], "UNMOUNT") == 0){
       if(!fs || !fs->mounted){
        printf("Nessun disco montato\n");
       }else{
        if(disk_unmount(fs)< 0){
            return -1;
        }else{
            return 1;
        }
       }
    }
    printf("errore shell unmoun\n");
    return - 1;
}

int shell_mkdir(char **args) {
    if( strcmp(args[0],"MKDIR") == 0 && args[1] == NULL){
        printf("Digitare MKDIR <name>\n");
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
    if(strstr(dir_name,".") != NULL){
        printf("Il nome della cartella non può contenere un punto\n");
        return -1;
    }
    DirHandle* dh = DirHandle_open(fs,dir_name,PERM_CREAT|PERM_EXCL);
    if(dh){
        if(DirHandle_close(fs,dh)<0) return -1;
        DirHandle_free(fs,dh);
    }else{
        printf("Errore creazione dh\n");
        return -1;
    }
    return -1;
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
        printf("Specificare nome/i del file: TOUCH <file>");
    }
    if(!fs || !fs->mounted){
        printf("Prima FORMAT, poi MOUNT\n");
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
        fh = FileHandle_open(fs,args[i],PERM_CREAT|PERM_EXCL|PERM_READ|PERM_WRITE);
        if(!fh){
            printf("Errore nella creazione di %s\n", args[i]);
            return -1;
        }
        if(FileHandle_close(fs,fh)<0){
            return -1;
        }
        FileHandle_free(fs,fh);
    }
    return 1;
}

int shell_cat(char **args) {
    if(strcmp(args[0], "CAT") == 0 && args[1] == NULL){
        printf("Inserire il nome del file da stampare\n");
        return -1;
    }
    if(!fs || !fs->mounted){
        printf("Prima FORMAT e MOUNT\n");
        return -1;
    }
    char* filename = args[1];
    Dir_Entry* target = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(!target){
        printf("Nessun file trovato\n");
        return -1;
    }else{
        if(target->is_dir == 0){
            printf("Selezionare un file, non una directory\n");
            return -1;
        }else{
            size_t len = target->file_size;
            char to_print[len+1];
            FileHandle* fh = FileHandle_open(fs,filename,PERM_READ);
            if(FileHandle_read(fs,fh,to_print,len) < 0){
                printf("Errore lettura da shell\n");
                return -1;
            }else{
                to_print[len] = '\0';
                fwrite(to_print,1,len,stdout);
                //printf("%s",to_print);
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
    if(!fs){
        printf("Prima FORMAT <disco>\n");
        return -1;
    }
    if(fs && !fs->mounted){
        printf("Prima MOUNT <disco>\n");
        return -1;
    }
    if(args[1] == NULL){
        printf("Contenuto dir:\n");
        Dir_Entry_curr_list(fs);
    }else{
        char* to_list = args[1];
        Dir_Entry* target = Dir_Entry_find_name(fs,to_list,fs->curr_dir);
        if(!target){
            printf("Dir non esistente");
        }else{
            Dir_Entry_list(fs,target->first_block);
        }
    }
    return 1;
}

int shell_append(char **args) {
    if(strcmp(args[0],"APPEND") == 0 && args[1] == NULL){
        printf("Occore specificare il file\n");
        return -1;
    }
    if(!fs || !fs->mounted){
        printf("Prima FORMAT e MOUNT\n");
        return -1;
    }
    char* filename = args[1];
    Dir_Entry* target = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(target){
        if(target->is_dir == 0){
            printf("Non è possibile usare append su una cartella\n");
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
            strcat(to_append, "\n");
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
            FileHandle_free(fs,fh);
            return 1;
        }
    }else if(!target){
        printf("Nessun file corrispondente a questo nome\n");
        return -1;
    }
    return -1;
}

int shell_rm(char **args) {
    return 1;
}

int shell_help(char **args) {
    printf("Comandi disponibili:\n");
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
        return 0;
    }
    printf("shell close error\n");
    return -1;
}