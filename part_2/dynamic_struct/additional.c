#include "additional.h"

void BuffClean() {
  while (getchar() != '\n')
    ;
}

void InputWrap(int received, int expected) {
  if (received != expected) {
    fprintf(stderr, "error: incorrected input\n");
    exit(EXIT_FAILURE);
  }
}