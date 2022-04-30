#include "header/kernel.h"
#include "header/filesystem.h"
#include "header/std_lib.h"
#include "header/program.h"

void lsCommand(byte curr_dir){
    struct node_filesystem node_fs_buffer;
    char* name[64];
    int i;
    int length;
    
    readSector(&(node_fs_buffer.nodes[0]),  FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    for(i = 0; i < 64; i++) {
        if(node_fs_buffer.nodes[i].parent_node_index == curr_dir && strlen(node_fs_buffer.nodes[i].name) != 0) {
            printString("\r\n");
            printString(node_fs_buffer.nodes[i].name);
        }
    }
}