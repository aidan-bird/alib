#ifndef ALIB_UTILS_H
#define ALIB_UTILS_H

/* 
 * Aidan Bird 2021
 *
 * contains useful functions and macros
 */ 

#include <stdio.h>
#include <time.h>

#include "array.h"


#define ROOT1_2F ((float)0.707106781186547)
#define PI_F ((float)3.141592653589793)
#define LEN(X) (sizeof(X) / sizeof((X)[0]))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define SQUARE(X) ((X) * (X))
#define DEG2RAD_F(X) ((X) * PI_F / 180)

/* 
 * Used for getting the execution time of EXPR
 * The time delta is stored at DOUBLE_PTR
 */
#define getExecTime(EXPR, DOUBLE_PTR) \
{ \
    clock_t start; \
    clock_t end; \
    start = clock(); \
    (EXPR); \
    end = clock(); \
    *(DOUBLE_PTR) = ((double) (end - start)) / CLOCKS_PER_SEC; \
}

const char *readTextFile(FILE *fp, int *outLength);
const void *readBinFile(FILE *fp, int *outLength);
void die(const char *msg);
Array *stringToIntArray(char *str);
void intSwap(int *restrict x, int *restrict y);
void *memdup(const void *src, size_t n);
size_t charCount(const void *src, size_t n, char b);
void printBinary(const void *src, size_t n);

#endif
