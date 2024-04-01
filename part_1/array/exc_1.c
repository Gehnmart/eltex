#include <stdint.h>
#include <stdio.h>

#include "general_io.h"
#include "general_utils.h"

#define FILL_MATRIX_LINE_BY_LINE 0
#define FILL_MATRIX_SNAIL 0x1

char FillSquareMatrix(unsigned N, char fill_type) {
  char err = 0;
  char matrix[N][N];
  int min_y = 0, min_x = 0;
  int max_y = N - 1, max_x = N - 1;


  int counter = 0;
  
  int y = 0;
  int x = 0;
  for(int i = 0; i < N; i++) {
    for (; x < max_x && counter <= N*N; ++x) {
      matrix[min_y][x] = ++counter;

    }
    ++min_y;
    --x;
    ++y;
    for (; y <= max_y && counter <= N*N; ++y) {
      matrix[y][max_x] = ++counter;
    }
    --max_x;
    --y;
    --x;
    for (; x >= min_x && counter <= N*N; --x) {
      matrix[max_y][x] = ++counter;
    }
    --max_y;
    x++;
    y--;
    for (; y >= min_y && counter <= N*N; --y) {
      matrix[y][min_x] = ++counter;
    }
    ++min_x;
    x++;
    y++;
  }

  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      printf("%d ", matrix[i][j]);
      char interval = GetNumberLength(N * N) - GetNumberLength(matrix[i][j]) + 1;
      FPutchInterval(stdout, interval);
    }
    printf("\n");
  }

  return err;
}

int main() {
  // int Number = 0;
  // char err = InputPositiveNumber(&Number);
  // char interval = GetNumberLength(N * N) - GetNumberLength(i) + 1;
      // PutSpaces(interval);

      // if (i % N == 0) {
      //   putchar('\n');
      // }
  FillSquareMatrix(2, FILL_MATRIX_SNAIL);
  printf("\n");

  return 0;
}