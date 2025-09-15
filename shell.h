#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Extern_fs.h"
#define MAX_ARGS 10
#define CMD_SIZE 13
#define MAX_ARG_LEN 1024
void shell_loop();
char** shell_split_line(char*);
char* shell_read();
int shell_exec(char**);
int shell_format(char **);
int shell_mkdir(char **);
int shell_cd(char **);
int shell_touch(char **);
int shell_cat(char **);
int shell_ls(char **);
int shell_append(char **);
int shell_rm(char **);
int shell_close(char **);
int shell_help(char **);
int shell_mount(char**);
int shell_unmount(char**);
int shell_chmod(char**);
int args_count(char**);
//int shell_pwd(char**);