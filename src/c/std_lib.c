#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C


int mod(int a, int n) {
    return a - n*(a/n);
}

void memcpy(byte *dest, byte *src, unsigned int n) {
    // Mengcopy n bytes yang ditunjuk src ke dest
}


void strcpy(char *dst, char *src) {
    // Melakukan penyalinan null terminated string
    int i = 0;
    do {
        dst[i] = src[i];
        i ++;
    } while (src[i] != '\0');// src[i] == '\0'
    dst[i] = '\0';
}


