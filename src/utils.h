#ifndef ALIB_UTILS_H
#define ALIB_UTILS_H

/* 
 * Aidan Bird 2021
 *
 * contains useful functions and macros
 */ 

#include <stdio.h>

#include "array.h"

/* get the length of an array */ 
#define LEN(X) (sizeof(X) / sizeof((X)[0]))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

const char *readTextFile(FILE *fp, int *outLength);
const void *readBinFile(FILE *fp, int *outLength);
void die(const char *msg);
Array *stringToIntArray(char *str);
void intSwap(int *restrict x, int *restrict y);

#endif
