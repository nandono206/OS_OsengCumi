#include "header/kernel.h"
#include "header/filesystem.h"
#include "header/std_lib.h"
#include "header/program.h"

void cd(byte parentIdx, char *targetName, byte *newParentIdx) {
    enum fs_retcode return_code;
    struct node_filesystem node_fs_buffer;
    bool found;
    int i;

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    if (strcmp(targetName, "..")) {
        if (parentIdx == FS_NODE_P_IDX_ROOT) {
            printString("cd: root doesn't have parent directory\r\n");
            return_code = FS_SUCCESS;
            return;
        }
        *newParentIdx = node_fs_buffer.nodes[parentIdx].parent_node_index;
        return;
    }
    if (strcmp(targetName, "/")) {
        *newParentIdx = FS_NODE_P_IDX_ROOT;
        return_code = FS_SUCCESS;
        return;
    }

    i = 0;
    found = false;
    
    while (i < 64 && !found) {
        if (node_fs_buffer.nodes[i].parent_node_index == parentIdx && strcmp(targetName, node_fs_buffer.nodes[i].name)) {
            *newParentIdx = i;
            return_code = FS_SUCCESS;
            found = true;
        }
        i++;
    }

    if (!found) {
        printString("cd: no such file or directory: ");
        printString(targetName);
        printString("\r\n");
        return_code = FS_R_NODE_NOT_FOUND;
    }
}