#include <string.h>
#include "shell.h"
#include "FS_info.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Extern_fs.h"
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
    for (int i = 0; args[i] != NULL; i++) {
    printf("arg[%d] = '%s'\n", i, args[i]);
    }

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
    if (strcmp(args[0], "FORMAT") == 0) {
        if (args[1] == NULL) {
            printf("Inserire nome disco\n");
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
    if( strcmp(args[0],"MOUNT") == 0){
        if(args[1] == NULL){
            printf("Inserire nome disco\n");
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
    return 1;
}

int shell_mkdir(char **args) {
    return 1;
}

int shell_cd(char **args) {
    return 1;
}

int shell_touch(char **args) {
    return 1;
}

int shell_cat(char **args) {
    return 1;
}

int shell_ls(char **args) {
    return 1;
}

int shell_append(char **args) {
    return 1;
}

int shell_rm(char **args) {
    return 1;
}

int shell_help(char **args) {
    printf("Comandi disponibili:\n");
    for(int i = 0; i < CMD_SIZE; i++) {
        printf("  %s\n", commands[i]);
    }
    return 1;
}

int shell_close(char **args) {
    return 1;
}