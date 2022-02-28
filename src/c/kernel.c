// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"

void readSector(char *buffer, int sector);

int main() {
  char buf[128];
  //clearScreen();
  makeInterrupt21();

  printString("Halo dunia!\r\n");
  readString(buf);
  printString(buf);

  while (true);
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
  int count;
  while (*(string + count) != '\0') {
    char ah = *(string + count);
    char al = 0xe;
    int ax = al * 256 + ah;

    interrupt(0x10, ax, 0, 0, 0);
    count++;
  }
}

void readString(char *string) {
  int idx = 0;
  char input_char = 0;
  while (input_char != '\r') { // selama input bukan enter
    input_char = interrupt(0x16, 0, 0, 0, 0);
    if (input_char == '\r') {
      string[idx] = '\0'; // akhiri dengan null
    } else if (input_char == '\b') { // input adalah backspace
        interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
				interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
				interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
        if (idx > 0) {
          string[idx] = '\0';
          idx --;
        }
    } else {
      string[idx] = input_char;
      idx ++;
      interrupt(0x10, 0xE00 + input_char, 0, 0, 0);
    }
  }
  // perpindahan line
  interrupt(0x10, 0xE00 + '\n', 0, 0, 0); 
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}