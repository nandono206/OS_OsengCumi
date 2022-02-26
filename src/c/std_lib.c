#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C


int mod(int a, int n) {
    return a - n*(a/n);
}

void strcpy(char *dst, char *src) {
    // Melakukan penyalinan null terminated string
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i ++;
    } // src[i] == '\0'
    dst[i] = '\0';
}


