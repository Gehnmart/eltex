#include "general_io.h"

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if ((err = InputNumber(&decimal)) == SUCCESS) {
    if (fprintf(stdout, "%d", CounterOfBits(decimal)) < 0) {
      err = OUTPUT_ERROR;
    }
  }
  if (err != SUCCESS) {
    if (err & INPUT_ERROR) {
      fprintf(stderr, "error: incorrect input, please enter only numbers");
    } else if (err & OUTPUT_ERROR) {
      fprintf(
          stderr,
          "error: incorrect output, please check your system and try again");
    } else {
      fprintf(stderr, "error: unknown error");
    }
  }
  printf("\n");

  return 0;
}
