#include <stdio.h>
#include <stdlib.h>

#define FAILURE 1
#define SUCCESS 0

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if (scanf("%d", &decimal) != 1) {
    err = FAILURE;
    fprintf(stderr, "error: incorect input");
  } else {
    PrintBit(decimal);
  }
  printf("\n");

  return err;
}

void PrintBit(int Number) {
  for (int i = sizeof(Number) * 8 - 1; i >= 0; --i) {
    printf("%d", ((Number >> i) & 1));
  }
}