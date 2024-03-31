#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define INPUT_ERROR 0x1
#define NEGATIVE_NUMBER_ERROR 0x2
#define OUTPUT_ERROR 0x4

#define PrintBit(Number) FPrintBit(stdout, Number)
#define InputPositiveNumber(Number) FInputPositiveNumber(stdin, Number)

char FInputPositiveNumber(FILE *stream, int *Number) {
  char err = SUCCESS;

  if (fscanf(stream, "%d", Number) == 1) {
    if (*Number < 0) {
      err |= NEGATIVE_NUMBER_ERROR;
    }
  } else {
    err |= INPUT_ERROR;
  }

  return err;
}

char FPrintBit(FILE *stream, int Number) {
  char err = SUCCESS;

  for (int i = sizeof(Number) * 8 - 1; i >= 0; --i) {
    if (fprintf(stream, "%d", ((Number >> i) & 1)) < 0) {
      err |= OUTPUT_ERROR;
      break;
    }
  }

  return err;
}

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if ((err = InputPositiveNumber(&decimal)) == SUCCESS) {
    err = PrintBit(decimal);
  }
  if (err != SUCCESS) {
    if (err & INPUT_ERROR) {
      fprintf(stderr,
              "error: incorrect input, please enter only positive number");
    } else if (err & NEGATIVE_NUMBER_ERROR) {
      fprintf(
          stderr,
          "error: incorrect number, is negative, please enter positive number");
    } else if (err & OUTPUT_ERROR) {
      fprintf(stderr,
              "error: incorrect output, please check your system and try again");
    } else {
      fprintf(stderr, "error: unknown error");
    }
  }
  printf("\n");

  return 0;
}
