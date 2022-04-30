#include "header/kernel.h"
#include "header/filesystem.h"
#include "header/std_lib.h"
#include "header/program.h"
#include "header/mv.h"

void mv(char *dir, char* file, byte cwd) {
    struct node_filesystem node_fs_buffer;
    byte new_dir;
    bool found = false;
    int node_idx = 0;

    readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    while (!found && node_idx < 64) {
        if (strcmp(dir, node_fs_buffer.nodes[node_idx].name) && node_fs_buffer.nodes[node_idx].parent_node_index == cwd) {
            new_dir = node_idx;
            found = true;
        }
        node_idx ++;
    }

    if (!found) {
        printString("Directory not found\n");
        return;
    }

    found = false;
    node_idx = 0;
    while (!found && node_idx < 64) {
        if (strcmp(file, node_fs_buffer.nodes[node_idx].name) && node_fs_buffer.nodes[node_idx].parent_node_index == cwd) {
            node_fs_buffer.nodes[node_idx].parent_node_index = new_dir;
            found = true;
        }
        node_idx ++;
    }
    if (!found) {
        printString("File not found\n");
        return;
    }

    writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}