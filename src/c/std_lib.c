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
    // Mengembalikan true jika string sama
    int i = 0;
    bool flag = true;
    while (flag) {
      if (s1[i] != s2[i]) {
        flag = false;
      } else if (s1[i] == '\0' && s2[i] == '\0') {
        break;
      }
      i ++;
    }
    return flag;
}

bool strCmpN(char *s1, char *s2, int N) {
   int i = 0;
   bool flag = true;
   while (!(s1[i] == '\0' && s2[i] == '\0') && i < N && flag) {
     if (s1[i] != s2[i]) {
       flag = false;
     }
     i ++;
   }
   return flag;
}

