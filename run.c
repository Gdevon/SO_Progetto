#include <stdlib.h>
#include "FS_info.h"
#include "shell.h"
#include "Extern_fs.h"
#include "Colors.h"
FileSystem* fs;
int main(int argc, char** argv){
    printf(RED "PER MAGGIORI INFORMAZIONI, DIGITA HELP\n\n\n" RESET);
    shell_loop();
}
