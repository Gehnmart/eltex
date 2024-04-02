#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "general_io.h"
#include "general_utils.h"

void PrintTriangleSideBySide(int N) {
  int triangle = 1;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", j + triangle < N ? 0 : 1);
    }
    triangle++;
    printf("\n");
  }
}

int main() {
  PrintTriangleSideBySide(10);
  printf("\n");

  return 0;
}