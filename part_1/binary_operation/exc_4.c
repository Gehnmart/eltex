#include "general_io.h"

int main() {
  char err = SUCCESS;
  int number = 0;
  int replace = 0;

  if ((err = InputNumber(&number)) == SUCCESS) {
    if ((err = InputNumber(&replace)) == SUCCESS) {
      SetByteInNumber(&number, replace, 2);
      err = PrintBitWithInterval(number, 8);
    }
  }

  return ErrorHandler(err);
}
