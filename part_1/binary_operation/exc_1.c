#include "general_io.h"

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if ((err = InputPositiveNumber(&decimal)) == SUCCESS) {
    err = PrintBit(decimal);
  }

  return ErrorHandler(err);
}
