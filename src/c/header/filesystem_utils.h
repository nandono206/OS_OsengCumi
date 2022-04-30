#include "std_type.h"
#include "std_lib.h"
#include "filesystem.h"

extern int interrupt(int int_number, int AX, int BX, int CX, int DX);

void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);

void fillKernelMap();
void fillMap();
void initializeNode();

int findNodeIdx(char* name, byte parent, struct node_filesystem *buffer);