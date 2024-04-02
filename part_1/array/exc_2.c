#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "general_io.h"
#include "general_utils.h"

void PrintReverseArray(int length, int array[]) {
  for (int i = length - 1; i >= 0; --i) {
    printf("%d ", array[i]);
  }
}

int main() {
  PrintReverseArray(6, (int[]){1, 2, 3, 4, 5, 6});
  printf("\n");

  return 0;
}