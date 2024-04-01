#include "general_utils.h"

static unsigned LenHelper(unsigned number) {
  if (number >= 1000000000)
    return 10;
  if (number >= 100000000)
    return 9;
  if (number >= 10000000)
    return 8;
  if (number >= 1000000)
    return 7;
  if (number >= 100000)
    return 6;
  if (number >= 10000)
    return 5;
  if (number >= 1000)
    return 4;
  if (number >= 100)
    return 3;
  if (number >= 10)
    return 2;
  return 1;
}

unsigned GetNumberLength(unsigned number) {
  return number < 0 ? LenHelper(-number) + 1 : LenHelper(number);
}