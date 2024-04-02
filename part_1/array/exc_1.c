#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "general_io.h"
#include "general_utils.h"

char PrintSquareMatrixSnail(unsigned N) {
  char err = 0;
  int matrix[N][N];
  memset(matrix, 0, sizeof(matrix));

  int counter = 0;

  char dy[] = {0, 1, 0, -1}; // direction y
  char dx[] = {1, 0, -1, 0}; // direction x
  char dir = 1;              // current direction

  int next_row = 0;
  int next_col = 0;
  for (int i = 0; i < N * N + 1; i++) {

    if (matrix[next_row][next_col] != 0 || next_row < 0 || next_row >= N ||
        next_col < 0 || next_col >= N) {
      next_row -= dy[dir - 1];
      next_col -= dx[dir - 1];
      dir = dir % 4 + 1;
    } else {
      matrix[next_row][next_col] = ++counter;
    }

    next_row += dy[dir - 1];
    next_col += dx[dir - 1];
  }

  PrintMatrix(N, N, matrix);

  return err;
}

void PrintSquareMatrixLineByLine(unsigned N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", i * N + j + 1);
    }
  }
}

int main() {
  PrintSquareMatrixSnail(10);
  printf("\n");

  return 0;
}