#include <stdlib.h>
#include "FS_info.h"
#include "shell.h"
#include "Extern.h"
#include "Colors.h"
FileSystem* fs;
char* pwd;
int pwd_pos;
int main(int argc, char** argv){
    pwd = (char*)malloc(sizeof(char)*1024);
    if(!pwd){
        printf("pwd non allocato correttamente\n");
        return -1;
    }   
    pwd[0] = '/';
    pwd[1] = '\0';
    pwd_pos = 1;
    printf(RED "PER MAGGIORI INFORMAZIONI, DIGITA HELP\n\n\n" RESET);
    shell_loop();
    if(pwd) free(pwd);
    return 1;
}
