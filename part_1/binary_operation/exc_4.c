#include "general_io.h"

int main() {
  int number = 0;
  int replace = 0;
  char err = SUCCESS;

  if ((err = InputNumber(&number)) == SUCCESS) {
    if ((err = InputNumber(&replace)) == SUCCESS) {
      SetByteInNumber(&number, replace, 2);
      err = PrintBit_8(number);
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
