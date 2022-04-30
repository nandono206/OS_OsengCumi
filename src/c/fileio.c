#include "header/fileio.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, (metadata->parent_index << 8) | 0x5, metadata->buffer, metadata->node_name, return_code);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
    interrupt(0x21, (metadata->parent_index << 8) | 0x4, metadata->buffer, metadata->node_name, return_code);
}
