#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "general_io.h"
#include "general_utils.h"

void PrintSquareMatrixLineByLine(int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", i * N + j + 1);
    }
    printf("\n");
  }
}

int main() {
  PrintSquareMatrixLineByLine(3);
  printf("\n");

  return 0;
}