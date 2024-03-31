#ifndef BINARY_OPERATION_GENERAL_IO_H
#define BINARY_OPERATION_GENERAL_IO_H

#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define INPUT_ERROR 0x1
#define POSITIVE_NUMBER_ERROR 0x2
#define NEGATIVE_NUMBER_ERROR 0x4
#define OUTPUT_ERROR 0x8

#define TYPE_ONLY_NEGATIVE_NUMBER 0
#define TYPE_ONLY_POSITIVE_NUMBER 1
#define TYPE_ANY_NUMBER 2

#define PrintBit(Number) FPrintBit(stdout, Number, 0, ' ', '\0')
#define PrintBit_4(Number) FPrintBit(stdout, Number, 4, ' ', '\0')
#define PrintBit_8(Number) FPrintBit(stdout, Number, 8, ' ', '\0')
#define InputNegativeNumber(Number)                                            \
  FInputNumber(stdin, Number, TYPE_ONLY_NEGATIVE_NUMBER)
#define InputPositiveNumber(Number)                                            \
  FInputNumber(stdin, Number, TYPE_ONLY_POSITIVE_NUMBER)
#define InputNumber(Number) FInputNumber(stdin, Number, TYPE_ANY_NUMBER)

char FInputNumber(FILE *stream, int *Number, int type);
char FPrintBit(FILE *stream, int Number, int Offset, char SpaceSymbol,
               char EndSymbol);
int CounterOfBits(int Number);
char SetByteInNumber(int *Number, char Byte, char ByteNumber);

#endif // BINARY_OPERATION_GENERAL_IO_H