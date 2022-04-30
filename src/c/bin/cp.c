#include "../header/kernel.h"
#include "../header/filesystem.h"
#include "../header/std_lib.h"
#include "../header/program.h"
#include "../header/cp.h"

void cp(char *filename, char *tocopyname, byte currdir) {
    byte buffer[8192];
    struct node_filesystem node_fs;
    struct node_entry node;
    struct file_metadata metadata;
    enum fs_retcode retcode;
    int i;

    clear(buffer, 8192);

    metadata.parent_index = currdir;
    metadata.buffer = buffer;
    metadata.node_name = filename;

    read(&metadata, &retcode);

    switch (retcode)
    {
    case FS_R_NODE_NOT_FOUND:
        printString("Error: File \"");
        printString(filename);
        printString("\" not found.\r\n");
        return;
    case FS_R_TYPE_IS_FOLDER:
        printString("Error: Can't copy directory.\r\n");
        return;
    }

    metadata.node_name = tocopyname;

    write(&metadata, &retcode);

    switch (retcode)
    {
    case FS_W_FILE_ALREADY_EXIST:
        printString("Error: File already exist \r\n");
        break;
    case FS_W_NOT_ENOUGH_STORAGE:
        printString("Error: Not enough space.\r\n");;
        break;
    case FS_W_MAXIMUM_NODE_ENTRY:
        printString("Error: Node reached maximum capacity.\r\n");
        break;
    case FS_W_MAXIMUM_SECTOR_ENTRY:
        printString("Error: Sector reached maximum capacity.\r\n");
        break;
    case FS_W_INVALID_FOLDER:
        printString("Error: Invalid folder.\r\n");
        break;
    }
}
