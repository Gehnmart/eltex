#include "general_io.h"

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if ((err = InputNegativeNumber(&decimal)) == SUCCESS) {
    err = PrintBit(decimal);
  }
  if (err != SUCCESS) {
    if (err & INPUT_ERROR) {
      fprintf(stderr, "error: incorrect input, please enter only numbers");
    } else if (err & NEGATIVE_NUMBER_ERROR) {
      fprintf(
          stderr,
          "error: incorrect number, is negative, please enter positive number");
    } else if (err & POSITIVE_NUMBER_ERROR) {
      fprintf(
          stderr,
          "error: incorrect number, is positive, please enter negative number");
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
