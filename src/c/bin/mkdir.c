#include "../header/kernel.h"
#include "../header/filesystem.h"
#include "../header/std_lib.h"
#include "../header/program.h"
#include "../header/mkdir.h"

void mkdir(char *name, byte cwd) {
    struct file_metadata metadata;
    enum fs_retcode retcode;

    metadata.filesize = 0;
    metadata.node_name = name;
    metadata.parent_index = cwd;

    write(&metadata, &retcode);

    if (retcode != FS_SUCCESS) {
        printString("Unable to make a new directory. \r\n");
    }
}
