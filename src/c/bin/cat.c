#include "header/kernel.h"
#include "header/filesystem.h"
#include "header/std_lib.h"
#include "header/program.h"

void cat(char* name, byte curr_dir) {
    enum fs_retcode return_code;
    struct file_metadata metadata;
    struct node_entry node;
    struct node_filesystem node_fs;
    byte buffer[8192];

    clear(buffer, 8192);

    metadata.parent_index = curr_dir;
    metadata.buffer = buffer;
    metadata.node_name = name;

    read(&metadata, &return_code);
    
    if (return_code == FS_R_NODE_NOT_FOUND) {
    	printString("File not found\n"); 	
    } else if (return_code == FS_R_TYPE_IS_FOLDER) {
    	printString("Not a file\n");
    }
    
    printString((char *)metadata.buffer);
    printString("\n");
}