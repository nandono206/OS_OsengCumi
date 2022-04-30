#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C

int div(int a, int b){
    return a/b;
}


int mod(int a, int n) {
    return a - n*(a/n);
}

void memcpy(byte *dest, byte *src, unsigned int n) {
    // Mengcopy n bytes yang ditunjuk src ke dest
    int i = 0;
    while (i < n) {
        dest[i] = src[i];
        i ++;
    }
}

void clear(byte *ptr, unsigned int n) {
    // Mengosongkan byte array yang memiliki panjang n
    int i = 0;
    while (i < n) {
        ptr[i] = 0x0;
        i ++;
    }
}