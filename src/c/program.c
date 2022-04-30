#include "header/program.h"

extern int interrupt(int num, int AX, int BX, int CX, int DX);

void exec(struct file_metadata meta, int segment) {
    enum fs_retcode retcode; 
    interrupt(0x21, (meta.parent_index << 8) | 0x6, meta.node_name, segment, &retcode);
}

void exit() {
  struct file_metadata meta;
  meta.node_name    = "shell";
  meta.parent_index = 0x00;
  exec(meta, 0x2000);
}
