// Kernel header

#include "std_type.h"
#include "std_lib.h"

// Fungsi bawaan
extern void putInMemory(int segment, int address, byte b);
extern int interrupt (int int_number, int AX, int BX, int CX, int DX);
extern void launchProgram(int segment);
extern void makeInterrupt21();

void handleInterrupt21(int AX, int BX, int CX, int DX);

// Implementasikan
void printString(char *string);
void readString(char *string);
void clearScreen();

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);

void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);

void fillMap();
void printCWD(char *path_str, byte curr_dir);
void cd(byte parentIdx, char *targetName, byte *newParentIdx);
void mkdir(char *name, byte cwd);
void cat(char* name, byte curr_dir);
void lsCommand(byte curr_dir);
void cp(char *filename, char *tocopyname, byte currdir);
void mv(char *dir, char* file, byte cwd);

void shell();

void executeProgram(struct file_metadata *metadata, int segment);
void splitCommand(char *input, char *com, char *args1, char *args2, char divider);
