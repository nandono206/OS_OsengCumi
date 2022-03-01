// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"
#include "header/std_lib.h"

int main() {
  char buf[512];
  clearScreen();
  makeInterrupt21();
  printString("Halo dunia!\r\n");
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
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
  int i = 0;
  while (string[i] != '\0') {
    interrupt(0x10, 0xe00 + string[i], 0, 0, 0);
    i++;
  }
  interrupt(0x10, 0xe00 + '\n', 0, 0, 0);
  interrupt(0x10, 0xe00 + '\r', 0, 0, 0);
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
