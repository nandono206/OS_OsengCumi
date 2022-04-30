#include "../header/filesystem_utils.h"
#include "../header/printCWD.h"

void printCWD(byte cwd)
{
  struct node_filesystem node_fs_buffer;
  byte path[9];
  byte dir = cwd;
  int len = 0;
  interrupt(0x21,0x02, &(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER, 0);
  interrupt(0x21,0x02, &(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1, 0);

  readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  while (dir != FS_NODE_P_IDX_ROOT)
  {
    path[len] = dir;
    len++;
    dir = node_fs_buffer.nodes[dir].parent_node_index;
  }

  while (len-- > 0)
  {
    interrupt(0x21,0x0, "/", 0, 0);
    interrupt(0x21,0x0, node_fs_buffer.nodes[path[len]].name, 0, 0);
  }
  interrupt(0x21,0x0, "/", 0, 0);;
}