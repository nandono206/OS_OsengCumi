extern int interrupt(int num, int AX, int BX, int CX, int DX);

int puts(char *str) {
  interrupt(0x21, 0x0, str, 0x0, 0x0);
  return 0;
}

char *gets(char *buff) {
  interrupt(0x21, 0x1, buff, 0x0, 0x0);
  return buff;
}
