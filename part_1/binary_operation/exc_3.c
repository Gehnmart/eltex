#include "general_io.h"

int main() {
  int decimal = 0;
  char err = SUCCESS;

  if ((err = InputNumber(&decimal)) == SUCCESS) {
    if (fprintf(stdout, "%d", CounterOfBits(decimal)) < 0) {
      err = OUTPUT_ERROR;
    }
  }

  return ErrorHandler(err);
}
