#ifndef BINARY_OPERATION_GENERAL_IO_H
#define BINARY_OPERATION_GENERAL_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SUCCESS 0
#define INPUT_ERROR 0x1
#define POSITIVE_NUMBER_ERROR 0x2
#define NEGATIVE_NUMBER_ERROR 0x4
#define OUTPUT_ERROR 0x8
#define OVERFLOW_ERROR 0x10
#define UNKNOWN_ERROR 0xE0

#define TYPE_ANY_NUMBER 0
#define TYPE_ONLY_NEGATIVE_NUMBER 0x1
#define TYPE_ONLY_POSITIVE_NUMBER 0x2

#define PrintBit(number) FPrintBit(stdout, number, 0, ' ', '\n')
#define PrintBitWithInterval(number, interval)                                     \
  FPrintBit(stdout, number, interval, ' ', '\n')
#define PutchInterval(interval) FPutchInterval(stdout, interval)
#define InputNegativeNumber(number)                                            \
  FInputNumber(stdin, number, TYPE_ONLY_NEGATIVE_NUMBER)
#define InputPositiveNumber(number)                                            \
  FInputNumber(stdin, number, TYPE_ONLY_POSITIVE_NUMBER)
#define InputNumber(number) FInputNumber(stdin, number, TYPE_ANY_NUMBER)

char FInputNumber(FILE *stream, int *number, char type);
char FPrintBit(FILE *stream, int number, int offset, char space_symbol,
               char end_symbol);
char FPutchInterval(FILE *stream, char interval);
char SetByteInNumber(int *number, char byte, char byte_number);
int CounterOfBits(int number);
int ErrorHandler(char err);

#endif // BINARY_OPERATION_GENERAL_IO_H