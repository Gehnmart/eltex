#include "general_io.h"

char FInputNumber(FILE *stream, int *number, char type) {
  int err = SUCCESS;

  if (fscanf(stream, "%d", number) == 1) {
    if (type & TYPE_ONLY_NEGATIVE_NUMBER) {
      if (*number > 0) {
        err |= POSITIVE_NUMBER_ERROR;
      }
    } else if (type & TYPE_ONLY_POSITIVE_NUMBER) {
      if (*number < 0) {
        err |= NEGATIVE_NUMBER_ERROR;
      }
    }
  } else {
    err |= INPUT_ERROR;
  }

  return err;
}

char FPrintBit(FILE *stream, int number, int offset, char space_symbol,
               char end_symbol) {
  char err = SUCCESS;

  for (int i = sizeof(number) * 8 - 1; i >= 0; --i) {
    if (fprintf(stream, "%d", ((number >> i) & 1)) < 0) {
      err |= OUTPUT_ERROR;
      break;
    }
    if (offset != 0) {
      if (i % offset == 0) {
        putc(space_symbol, stream);
      }
    }
  }
  putc(end_symbol, stream);

  return err;
}

char FPutchInterval(FILE *stream, char interval) {
  char err = 0;
  for (char k = 0; k < interval; ++k) {
    if (putchar(' ') < 0) {
      err |= OUTPUT_ERROR;
      break;
    }
  }

  return err;
}

char SetByteInNumber(int *number, char byte, char byte_number) {
  char err = SUCCESS;
  if (byte_number >= sizeof(*number)) {
    err |= OVERFLOW_ERROR;
  }
  char *ptr = (char *)number;
  ptr += byte_number;
  *ptr = byte;

  return err;
}

int CounterOfBits(int number) {
  int counter = 0;
  for (int i = sizeof(number) * 8 - 1; i >= 0; --i) {
    if ((number >> i) & 1) {
      ++counter;
    }
  }
  return counter;
}

int ErrorHandler(char err){
  int error_count = 0;
  if (err != SUCCESS) {
    if (err & INPUT_ERROR) {
      ++error_count;
      fprintf(stderr, "error: incorrect input, please enter only numbers");
    } if (err & NEGATIVE_NUMBER_ERROR) {
      ++error_count;
      fprintf(
          stderr,
          "error: incorrect number, is negative, please enter positive number");
    } if (err & POSITIVE_NUMBER_ERROR) {
      ++error_count;
      fprintf(
          stderr,
          "error: incorrect number, is positive, please enter negative number");
    } if (err & OUTPUT_ERROR) {
      ++error_count;
      fprintf(
          stderr,
          "error: incorrect output, please check your system and try again");
    } if (err & UNKNOWN_ERROR) {
      ++error_count;
      fprintf(stderr, "error: unknown error");
    }
  }
  
  return error_count;
}