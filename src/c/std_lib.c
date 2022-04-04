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

unsigned int strlen(char *string) {
    int i = 0;
    while (string[i] != '\0')
        i++;
    return i;
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

bool strcmp(char *s1, char *s2) {
    int i = 0;
    bool isEqual = true;
    do {
        if (s1[i] != s2[i]) {
            isEqual = false;
        }
        i ++;
    } while (isEqual && s1[i] != '\0' && s2[i] != '\0');
    return isEqual;
}
    // Mengembalikan true jika string sama
    
bool subStrCmp(char *s1, char *s2, int idx) {
    // Mengembalikan true jika string sama sampai dengan index ke-idx
    int i = 0;
    int j = 0;
    bool isEqual = true;
    do {
        if (s1[idx] != s2[idx]) {
            isEqual = false;
        }
        i ++;
        j ++;
    } while (isEqual && s1[idx] == '\0' && s2[idx] == '\0' && j <= idx);
    return isEqual;
}
