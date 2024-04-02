#ifndef BINARY_OPERATION_GENERAL_IO_H
#define BINARY_OPERATION_GENERAL_IO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "general_utils.h"

// Error types
#define SUCCESS 0
#define INPUT_ERROR 0x1
#define POSITIVE_NUMBER_ERROR 0x2
#define NEGATIVE_NUMBER_ERROR 0x4
#define OUTPUT_ERROR 0x8
#define OVERFLOW_ERROR 0x10
#define UNKNOWN_ERROR 0xE0

// Number types
#define TYPE_ANY_NUMBER 0
#define TYPE_ONLY_NEGATIVE_NUMBER 0x1
#define TYPE_ONLY_POSITIVE_NUMBER 0x2

/**
 * @brief Output bits of a number to stdout
 * @param number - integer number
 * @return error code
 */
#define PrintBit(number) FPrintBit(stdout, number, 0, ' ', '\n')

/**
 * @brief Output bits of a number to stdout with interval
 * @param number - integer number
 * @param interval - number of bits to set the interval
 * @return error code
 */
#define PrintBitWithInterval(number, interval) \
  FPrintBit(stdout, number, interval, ' ', '\n')

/**
 * @brief Print an interval to stdout
 * @param interval - number of spaces to print
 * @return error code
 */
#define PutchInterval(interval) FPutchInterval(stdout, interval)

/**
 * @brief Output a matrix to stdout
 * @param rows - number of rows
 * @param columns - number of columns
 * @param matrix - matrix
 */
#define PrintMatrix(rows, columns, matrix) \
  FPrintMatrix(stdout, rows, columns, matrix)

/**
 * @brief Input a negative number from stdin
 * @param number - pointer to int where to store the input number
 * @return error code
 */
#define InputNegativeNumber(number) \
  FInputNumber(stdin, number, TYPE_ONLY_NEGATIVE_NUMBER)

/**
 * @brief Input a positive number from stdin
 * @param number - pointer to int where to store the input number
 * @return error code
 */
#define InputPositiveNumber(number) \
  FInputNumber(stdin, number, TYPE_ONLY_POSITIVE_NUMBER)

/**
 * @brief Input a number from stdin
 * @param number - pointer to int where to store the input number
 * @return error code
 */
#define InputNumber(number) FInputNumber(stdin, number, TYPE_ANY_NUMBER)

/**
 * @brief Function to input a number with specified constraints from a stream
 * @param stream - pointer to input stream (e.g., stdin)
 * @param number - pointer to variable where the input number will be stored
 * @param type - type of number (TYPE_ANY_NUMBER, TYPE_ONLY_NEGATIVE_NUMBER,
 * TYPE_ONLY_POSITIVE_NUMBER)
 * @return error code
 */
char FInputNumber(FILE *stream, int *number, char type);

/**
 * @brief Function to print the bits of a number to a stream
 * @param stream - pointer to output stream (e.g., stdout)
 * @param number - integer number whose bits will be printed
 * @param interval - bit interval (from 0 to 31)
 * @param space_symbol - symbol for spacing between bits
 * @param end_symbol - symbol for ending the output
 * @return error code
 */
char FPrintBit(FILE *stream, int number, int interval, char space_symbol,
               char end_symbol);

/**
 * @brief Function to print spaces to a stream
 * @param stream - pointer to output stream (e.g., stdout)
 * @param interval - number of spaces to print
 * @return error code
 */
char FPutchInterval(FILE *stream, char interval);

/**
 * @brief Function to print a matrix to a stream
 * @param stream - pointer to output stream (e.g., stdout)
 * @param rows - number of rows in the matrix
 * @param columns - number of columns in the matrix
 * @param matrix - two-dimensional array representing the matrix
 */
void FPrintMatrix(FILE *stream, int rows, int columns, int matrix[][columns]);

/**
 * @brief Function to set a byte in a number
 * @param number - integer number
 * @param byte - byte to set
 * @param byte_number - byte number to set
 * @return error code
 */
char SetByteInNumber(int *number, char byte, char byte_number);

/**
 * @brief Function to count the number of bits in a number
 * @param number - integer number
 * @return counts of bits
 */
int CounterOfBits(int number);

/**
 * @brief Function to handle errors
 * @param err - error code
 * @return error count
 */
int ErrorHandler(char err);

#endif  // BINARY_OPERATION_GENERAL_IO_H
