// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"
#include "header/std_lib.h"
#include "header/filesystem.h"
#include "header/std_type.h"

int main() {
  char buf[512];
  clearScreen();
  fillMap();
  makeInterrupt21();
  printString("Halo dunia!\r\n");
  shell();
  while(true) {
    readString(buf);
    printString(buf);
  }
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
    switch (AX) {
        case 0x0:
            printString(BX);
            break;
        case 0x1:
            readString(BX);
            break;
        case 0x2:
            readSector(BX, CX);
            break;
        case 0x3:
            writeSector(BX, CX);
            break;
        case 0x4:
            read(BX, CX);
            break;
        case 0x5:
            write(BX, CX);
            break;
        case 0x6:
            executeProgram(BX, CX);
            break;
        default:
            printString("Invalid Interrupt");
    }
}

void executeProgram(struct file_metadata *metadata, int segment)
{
    enum fs_retcode fs_ret;
    byte buf[8192];

    metadata->buffer = buf;
    read(metadata, &fs_ret);
    if (fs_ret == FS_SUCCESS)
    {
        int i = 0;
        for (i = 0; i < 8192; i++)
        {
            if (i < metadata->filesize)
                putInMemory(segment, i, metadata->buffer[i]);
            else
                putInMemory(segment, i, 0x00);
        }
        launchProgram(segment);
    }
    else
        printString("exec: file not found\r\n");
}

void shell() {
  char input_buf[64];
  char command[64];
  char args1[64];
  char args2[64];
  char path_str[128];
  char fullCommand[10][64];
  byte current_dir = FS_NODE_P_IDX_ROOT;
  int res;
  struct file_metadata metadata;
  struct file_metadata meta;
  enum fs_retcode return_code;

  while (true) {
    printString("OS@IF2230:");
    printCWD(path_str, current_dir);
    printString("$");
    readString(input_buf);
    printString("\n");
    
    splitCommand(input_buf, args2, args1, command, ' ');
    // printString(command);
    // printString(args1);
    // printString(args2);
    // printString(command);
    // printString("\n");
    // printString(args);
    
    /*if (res != -1) {
	printString(" Valid!\r\n");
    	if (strCmpN(fullCommand[0], "\r\n", 5)) {
	       // Utility cd
	       printString(" Berhasil!\r\n");
	       cat("file_luar", current_dir);
	       */
	       //metadata.node_name = fullCommand[1];
	    
   // elif untuk command lainnya
      if (strcmp(command, "\r\ncat")){
        cat(args1, current_dir);
      }
      else if (strcmp(command, "\r\nls")) {
        lsCommand(current_dir);
        printString("\n");
        // printString("inside command\r\n");
      }
      else if (strcmp(command, "\r\ncd")){
        // printString("inside cd command\r\n");
        cd(current_dir, args1, &current_dir);
      }
      else if (strcmp(command, "\r\nmkdir")){
        // printString("inside cd command\r\n");
        mkdir(args1, current_dir);
      } else if (strcmp(command, "\r\ncp")) {
      	cp(args1, args2, current_dir);
      } else if (strcmp(command, "\r\nmv")) {
      	mv(args2, args1, current_dir);
      } else if (strcmp(command, "\r\ntest")) {
        meta.node_name    = "shell";
        meta.parent_index = 0xFF;
        executeProgram(&meta, 0x2000);
    }

      else {
        printString("Unknown command\r\n");
      }
    }    
    // else 
    //   printString("Unknown command\r\n");
    printString("\r\n");
    //ls(current_dir);
 
  
}



void printString(char *string) {
  int i = 0;
  while (string[i] != '\0') {
    interrupt(0x10, 0xe00 + string[i], 0, 0, 0);
    if (string[i] == '\n') {
        interrupt(0x10, 0xe00+0xd, 0x000F, 0, 0);
    }
    i++;
  }
  // interrupt(0x10, 0xe00 + '\n', 0, 0, 0);
  // interrupt(0x10, 0xe00 + '\r', 0, 0, 0);
}


void readString(char *string){
  int i = 2;
  int ax = 0xe << 8;
  string[0] = '\r'; // enter
  string[1] = '\n'; // newline
  while(true) {
    char input = interrupt(0x16, 0x0, 0, 0, 0);
    if(input == '\r'){ // input adalah enter
      string[i] = '\0'; // endstring
      string[i + 1] = '\r';
      string[i + 2] = '\n';
      break;
    } else if (input == '\b') { // input adalah backspace
        if (i > 2) {
          interrupt(0x10, 0xe00 + '\b', 0, 0, 0);
          interrupt(0x10, 0xe00 + '\0', 0, 0, 0);
          interrupt(0x10, 0xe00 + '\b', 0, 0, 0);
          string[i] = '\0';
          i--;
        } else {
          i = 2;
	  string[0] = '\r';
	  string[1] = '\n';
	}
    } else {
        string[i] = input;
	ax += input;
	interrupt(0x10, ax, 0x11, 0, 0);
	ax -= input;
	i++;
    }
  }
}


void clearScreen() {
   int AX, BX, DX;
   AX = (0x07 << 8) | 0x00;
   BX = (0x07 << 8) | 0;
   DX = (24 << 8) | 79;
   interrupt(0x10, AX, BX, 0, DX);
   AX = (0x02 << 8) | 0x00;
   interrupt(0x10, AX, 0, 0, 0);
}
