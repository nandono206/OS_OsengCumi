// Kernel header

#include "std_type.h"
#include "std_lib.h"

// Fungsi bawaan
extern void putInMemory(int segment, int address, byte b);
extern int interrupt (int int_number, int AX, int BX, int CX, int DX);
extern void makeInterrupt21();

void handleInterrupt21(int AX, int BX, int CX, int DX);

// Implementasikan
void printString(char *string);
void readString(char *string);
void clearScreen();

void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);

void fillMap();
void printCWD(char *path_str, byte curr_dir);
void ls(byte curr_dir);

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);

void shell();

void cat(char *name, byte cwd);
void splitCommand(char *buffer, char *first, char *second, char delimiter);
