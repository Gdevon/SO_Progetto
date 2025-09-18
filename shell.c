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
int pres = 0;
char* commands[] = {"FORMAT","MOUNT","UNMOUNT",
"MKDIR","CD","TOUCH","CAT","LS","APPEND","RM","HELP","CLOSE","CHMOD","PWD"};
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
    &shell_chmod,
    //&shell_pwd
};
void shell_loop(){
    char *line;
    char **args;
    int status;
    do{
    if(!pwd){
        printf(GRN "$hell:" RESET " ");
    }else if(strcmp(pwd, "/") == 0){
        printf(GRN "$hell:" BLU "~" RESET WHITE "$ " RESET " ");
    }else{
        printf(GRN "$hell:" BLU "~%s " RESET WHITE "$:" RESET " ", pwd);
    }
    fflush(stdout);
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
    for(int i=0;i<CMD_SIZE;++i){
        if(strcmp(args[0], commands[i]) == 0){
            return (*cmd_pointer[i])(args);
        }
    }
    puts(BLU "Comando non supportato, per maggiori informazoni : HELP" RESET);
    return 1;
}
char* shell_read(){
    size_t pos = 0;
    size_t new_size = 1024;
    char* line = (char*)malloc(MAX_ARG_LEN);
    if(!line){
        printf(RED"Problemi di allocazione in shell_read\n"RESET);
        return NULL;
    }
    int c;
    while ( (c=getchar()) != '\n' && (c != EOF)){
        line[pos] = c;
        pos++;
        if(pos >= new_size-1){
            new_size*=2;
            line = realloc(line,new_size);
            if(!line){
                printf(RED"Errore nella realloc\n"RESET);
                return NULL;
            }
        }
    }
    line[pos] = '\0';
    return line;
}
int shell_format(char **args) {
    if(args_count(args) != 2){
        printf(RED "Uso corretto: FORMAT <diskname>\n"RESET);
        printf(BLU"Si consiglia di inserire .fs al nome del disco\n"RESET);
        return -1;
    }
    char *fs_name = args[1];
    int fd = open(fs_name, O_RDONLY);
    if (fd >= 0) {
    printf(GRN"Disco '%s' esistente, formatto...\n",fs_name);
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

int shell_mount(char **args) {
    if(args_count(args) != 2){
        printf(RED"Uso corretto: MOUNT <disk>\n"RESET);
        return -1;
    }
    if(fs && fs->mounted){
        printf(BLU "Risulta già montato un disco con il nome %s\n"RESET,args[1]);
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

int shell_unmount(char **args) {
    if(args_count(args) != 1){
    printf(RED"Digitare solo UNMOUNT\n"RESET);
        return -1;
    }
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
    printf(RED"Errore shell unmount\n"RESET);
    return - 1;
}

int shell_mkdir(char **args) {
    if(args_count(args)<= 1){
        printf(RED"Specificare nome/i della/e cartella/e: MKDIR <dir>"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Per creare cartelle, prima occorre eseguire MOUNT <disk>\n"RESET);
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
    if(args_count(args)!= 2){
        printf(RED"Inserire la cartella da visitare: CD <dest>\n"RESET);
        return -1;
    }
    if(!fs){
        printf(RED"Prima usare FORMAT <disco>\n"RESET);
        return -1;
    }
    if(fs && !fs->mounted){
        printf(RED"Prima fare MOUNT <disco>\n"RESET);
        return -1;
    }
    char* dir_name = args[1];
    if(Dir_Entry_change(fs,dir_name) < 0){
        printf(RED"Errore nel cambio directory, esiste?\n"RESET);
        return -1;
    }
    if (strcmp(dir_name, "/") == 0) {
        pwd[0] = '/';
        pwd[1] = '\0';
        pwd_pos = 1;
    }else if (strcmp(dir_name, "..") == 0) {
        if (pwd_pos > 1) {  
            int i = pwd_pos - 2; 
            while (i > 0 && pwd[i] != '/') i--;
            if(i == 0){
                pwd[1] = '\0';
                pwd_pos = 1;
            }else{
                pwd[i] = '\0';
                pwd_pos = i;
                if(pwd_pos == 0){
                    pwd[0] = '/';
                    pwd[1] = '\0';
                    pwd_pos = 1;
                }
            }
        }
    }else{
        int len = strlen(dir_name);
        if (pwd_pos > 1) {          
            pwd[pwd_pos] = '/';
            pwd_pos++;
        }
        strcpy(pwd+pwd_pos,dir_name);
        pwd_pos += len;            
    }
    return 1;
}

int shell_touch(char **args) {
    if(args_count(args)<= 1){
        printf(RED"Specificare nome/i del file: TOUCH <file>"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT, poi MOUNT <disk>\n"RESET);
        return -1;
    }
    int start = 1;
    int perms = 0;
    if( strcmp(args[1],"-P") == 0){
        perms = 1;
        if(args[2] == NULL){
            printf(RED"Specificare almeno un file\n"RESET);
            return -1;
        }
        start = 2;
    }
    int num_of_files = 0;
    for(int i = start;args[i] != NULL;++i){
        num_of_files++;;
    }
    //printf("Devo creare %d files\n", num_of_files);
    FileHandle* fh;
    for( int i = start ;args[i] != NULL;++i){
        fh = FileHandle_open(fs,args[i],PERM_CREAT|PERM_EXCL);
        if(perms)FileHandle_change_perm(fh,PERM_WRITE|PERM_READ);
        if(!fh){
            printf(RED"Errore nella creazione di %s, il file già esiste\n", args[i] );
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
    if(args_count(args) != 2){
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
            if(fh){
                FileHandle_read(fs,fh,to_print,len);
                to_print[len] = '\0';
                fwrite(to_print,1,len,stdout);
                printf("\n");
                FileHandle_close(fs,fh);
                return 1;
            }
            else if(!fh){
                printf(BLU"Usa: CHMOD %s PERM_READ\n"RESET, filename);
                return -1;
            }
        }
    }
    return 1;
}

int shell_ls(char **args) {
    if(args_count(args) > 2){
        printf(RED "uso corretto: LS <filename> (per listare permessi) / LS / LS <dir>\n"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima è necessario creare o fare la mount del disco\n"RESET);
        return -1;
    }
    printf("------------------------------------------\n");
    if(args[1] == NULL){
        Dir_Entry_curr_list(fs);
    }else{
        char* to_list = args[1];
        Dir_Entry* target = Dir_Entry_find_name(fs,to_list,fs->curr_dir);
        
        if(!target){
            printf(RED"Impossibile trovare un elemento con questo nome\n"RESET);
        }
        else if(target->is_dir == 1){
            Dir_Entry_list_aux(target);
        }else{
            Dir_Entry_list(fs,target->first_block);
        }
    }
    printf("------------------------------------------\n");
    return 1;
}

int shell_append(char **args) {
    if(args_count(args) <= 2){
        printf(RED"Uso corretto: APPEND <file> <testo>\n"RESET);
        return -1;
    }
    if(!fs || !fs->mounted){
        printf(RED"Prima FORMAT e MOUNT\n"RESET);
        return -1;
    }
    char* filename = args[1];
    Dir_Entry* target = Dir_Entry_find_name(fs,filename,fs->curr_dir);
    if(!target){
        printf(RED"Non ho trovato nessun file con questo nome\n"RESET);
        return -1;
    }
    if(target->is_dir ==0){
        printf(RED"Non è possibile usare APPEND su una cartella\n"RESET);
        return -1;
    }
    size_t len = 0;
    for(int i = 2;args[i] != NULL;++i){
        len += strlen(args[i]);
        if(args[i+1] != NULL ){
            len++;
        }
    }
    char* to_append = (char*)calloc(len+1,sizeof(char));
    if(!to_append){
        printf("Problemi nella calloc\n");
        return -1;
    }
    for(int i=2;args[i] != NULL;++i){
        strcat(to_append,args[i]);
        if(args[i+1]!= NULL){
            strcat(to_append, " ");
        }
    }
    char* pos = to_append;
    while((pos = strstr(pos,"\\n")) != NULL){
        *pos = '\n';
        memmove(pos+1,pos+2,strlen(pos+2) + 1);
        pos++;
    }
    FileHandle* fh = FileHandle_open(fs,filename,PERM_WRITE);
    if(!fh){
        printf(BLU"Usa: CHMOD %s PERM_WRITE\n"RESET, filename);
        free(to_append);
        return -1;
    }
    if(FileHandle_write(fs,fh,to_append,strlen(to_append)) < 0){
        printf(RED"Problemi in scrittura da shell\n"RESET);
        FileHandle_close(fs,fh);
        free(to_append);
        return -1;
    }  
    if(FileHandle_close(fs,fh)<0) {
        free(to_append);
        return -1;
    }
    printf(BLU"Testo aggiunto con successo\n"RESET);
    free(to_append);
    return 1;
}

int shell_rm(char **args) {
    if (args_count(args)<=1) {
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
                    printf(BLU"Operazione %s eseguita\n", handle_name);
                } else {
                    printf(BLU"Eliminazione di %s annullata\n", handle_name);
                    continue;
                }
            } else {
                if (DirHandle_delete(fs, handle_name) < 0) {
                    printf(RED"Errore nell'eliminazione della directory, se contiene elementi : RM -RF <dir> %s\n", handle_name);
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
    int* a = &pres;
    if(*a == 0){
        printf("Per iniziare: FORMAT <nomedisco.fs> -> MOUNT <nomedisco.fs>, oppure solo MOUNT <nomedisco.fs>, se già è stato creato in precedenza. Da qui possono essere eseguiti i comandi disponibili digitando HELP. Per chiudere -> UNMOUNT -> CLOSE\n");
        *a = 1;
    }
    printf(BLU"Comandi disponibili:\n\\n"RESET);
    printf(
    "FORMAT <disco>\n"
    "MOUNT <disco>\n"
    "UNMOUNT \n"
    "CLOSE\n"
    "\n"
    "TOUCH [-P] <file/s> (inserire -P per attivare i permessi al momento della creazione)\n"
    "CAT <file>\n"
    "APPEND <file> <testo>\n"
    "RM [-RF] <files/cartelle>\n"
    "CHMOD <permessi>\n"
    "\n"
    "MKDIR <dir>\n"
    "CD <dest>\n"
    "LS / LS <dir/file>\n"
    "\n"
    "HELP\n"
);
return 1;
}
int shell_close(char **args) {
    if(args_count(args)!= 1){
        printf(RED "Uso corretto: UNMOUNT\n");
        return -1;
    }
    if(!fs){
        printf("Nessun fs da chiudere\n");
        return 0;
    }
    if(fs->mounted){
        //if(disk_unmount(fs) < 0){
        //    return -1;
        //}
        printf(RED"Prima eseguire UNMOUNT DISCO\n"RESET);
        return -1;
    }
    fs_free(&fs);
    fs = NULL;
    printf(BLU "Chiusura effettuata correttamente\n" RESET);
    return 0;
}
int shell_chmod(char** args){
    if(args_count(args)!=3){
        printf(RED"Uso corretto: CHMOD <file> <perms>\n"RESET);
        return -1;
    }
    char* filename = args[1];
    char** perms = (char**)malloc(sizeof(char*)*2);
    char* token = strtok(args[2],"|\n");
    int count = 0;
       while(count<=1 && token != NULL){
        perms[count] = malloc(strlen(token)+1);
        strcpy(perms[count],token);
        count += 1;
        token = strtok(NULL,"|\n");
    }
    FileHandle* fh;
    if(count == 1){
        if(strcmp(perms[0],"PERM_READ") == 0){
            int p = PERM_READ;
            fh = FileHandle_open(fs,filename,PERM_NO);
            if(fh){
                FileHandle_change_perm(fh,p);
                if(FileHandle_close(fs,fh)<0){
                    return -1;
                }
            }else{
                printf(RED"File Non trovato\n"RESET);
            }
        }else if(strcmp(perms[0],"PERM_WRITE") ==0){
            int p = PERM_WRITE;
            fh = FileHandle_open(fs,filename,PERM_NO);
            if(fh){
                FileHandle_change_perm(fh,p);
                if(FileHandle_close(fs,fh)<0){
                    return -1;
                }
            }else{
                printf(RED"File Non trovato\n"RESET);
            }
        }else{
            printf(BLU"Al momento sono supportati due permessi: PERM_READ|PERM_WRITE\n"RESET);
            return -1;
        }
    }else{
        if( ((strcmp(perms[0],"PERM_READ") == 0 ) && (strcmp(perms[1],"PERM_WRITE") == 0) )
        || ((strcmp(perms[0],"PERM_WRITE") == 0 && (strcmp(perms[1],"PERM_READ") == 0)))){
            int p = PERM_READ|PERM_WRITE;
            fh = FileHandle_open(fs,filename,PERM_NO);
            if(fh){
                FileHandle_change_perm(fh,p);
                if(FileHandle_close(fs,fh) <0 ) return -1;
            }else{
                printf(RED"File Non trovato\n"RESET);
            }
        }else{
            printf(BLU "Permessi disponibili da impostare (una volta sola) PERM_READ|PERM_WRITE\n"RESET);
        }
    }
    for(int i = 0; i < count;++i) free(perms[i]);
    free(perms);
    printf(BLU"Permessi cambiati con successo\n"RESET);
    return 1;
}
int args_count(char** args){
    int c = 0;
    for(int i = 0;args[i] != NULL;++i){
        c+=1;
    }
    return c;
}
/*int shell_pwd(char** args){
    if(args_count(args) != 1){
        return -1;
    }
    printf("La working directory è %s\n",pwd);
    return 1;
}*/