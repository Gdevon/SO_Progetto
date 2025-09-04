#include <stdlib.h>
#include "FS_info.h"
#include "shell.h"
#include "Extern_fs.h"
FileSystem* fs;
int main(int argc, char** argv){
    //fs = fs_init();
    //if(!fs){
    //    print_error(FS_NOTINIT);
    //    return -1;
    //}
    shell_loop();
}
