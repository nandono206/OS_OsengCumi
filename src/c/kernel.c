// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"

void readSector(char *buffer, int sector);

int main() {
  char *string = "Hai"; // Deklarasikan variabel pada awal scope
  int i = 0;

  for (i = 0; i < 3; i++) {
    byte warna = 0xD;
    putInMemory(0xB000, 0x8000 + 2*i, string[i]);
    putInMemory(0xB000, 0x8001 + 2*i, warna);
  }

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
