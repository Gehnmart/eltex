#include "general_io.h"

char FInputNumber(FILE *stream, int *Number, int type) {
  char err = SUCCESS;

  if (fscanf(stream, "%d", Number) == 1) {
    switch (type) {
    case TYPE_ONLY_NEGATIVE_NUMBER:
      if (*Number > 0) {
        err |= POSITIVE_NUMBER_ERROR;
      }
      break;
    case TYPE_ONLY_POSITIVE_NUMBER:
      if (*Number < 0) {
        err |= NEGATIVE_NUMBER_ERROR;
      }
      break;
    case TYPE_ANY_NUMBER:
      break;
    default:
      fprintf(stderr, "error: unedfined type, type should be 0, 1 or 2");
      break;
    }
  } else {
    err |= INPUT_ERROR;
  }

  return err;
}

char FPrintBit(FILE *stream, int Number, int Offset, char SpaceSymbol,
               char EndSymbol) {
  char err = SUCCESS;

  for (int i = sizeof(Number) * 8 - 1; i >= 0; --i) {
    if (fprintf(stream, "%d", ((Number >> i) & 1)) < 0) {
      err |= OUTPUT_ERROR;
      break;
    }
    if (Offset != 0) {
      if (i % Offset == 0) {
        fprintf(stream, "%c", SpaceSymbol);
      }
    }
  }

  return err;
}

char SetByteInNumber(int *Number, char Byte, char ByteNumber) {
  char *ptr = (char *)Number;
  ptr += ByteNumber;
  *ptr = Byte;
}

int CounterOfBits(int Number) {
  int counter = 0;
  for (int i = sizeof(Number) * 8 - 1; i >= 0; --i) {
    if ((Number >> i) & 1) {
      ++counter;
    }
  }
  return counter;
}