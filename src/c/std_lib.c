#include "header/std_lib.h"

// Opsional : Tambahkan implementasi kode C


int mod(int a, int n) {
    return a - n*(a/n);
}
int div(int a, int b){
    return a/b;
}

unsigned int strlen(char *string) {
    unsigned int i = 0;
    while (string[i] != '\0')
        i++;
    return i;
}

void memcpy(byte *dest, byte *src, unsigned int n) {
    // Mengcopy n bytes yang ditunjuk src ke dest
    int i = 0;
    while (i < n) {
        dest[i] = src[i];
        i ++;
    }
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

void clear(byte *ptr, unsigned int n) {
    // Mengosongkan byte array yang memiliki panjang n
    int i = 0;
    while (i < n) {
        ptr[i] = 0x0;
        i ++;
    }
}