/* Aidan Bird 2021 */ 

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "array.h"

#define BLOCKSIZE 1024

Array *readFileHelper(FILE *fp, int *outSize);

void
intSwap(int *restrict x, int *restrict y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

/* 
 * print an error message and terminate the program
 */ 
void
die(const char *msg)
{
    fputs(msg, stderr);
    exit(-1);
}

/*
 * Read all text from a file pointer.
 * If outLength is not NULL, then it shall be overwritten with the length of 
 * the input read.
 */ 
const char *
readTextFile(FILE *fp, int *outLength)
{
    size_t tmp;
    char buf[BLOCKSIZE];
    char *ret;
    Array *arr;
    int len;

    if (!(arr = readFileHelper(fp, NULL)))
        goto error1;
    len = arrayToString(arr, &ret);
    if (outLength)
        *outLength = len;
    return ret;
error1:;
    return NULL;
}

/* XXX experimental */
const void *
readBinFile(FILE *fp, int *outLength)
{
    size_t tmp;
    char buf[BLOCKSIZE];
    Array *ret;
    int len;

    if (!(ret = readFileHelper(fp, &len)))
        goto error1;
    arrayToRaw(ret, len);
    if (outLength)
        *outLength = len;
    return ret;
error1:;
    return NULL;
}

/* XXX experimental */
Array *
readFileHelper(FILE *fp, int *outSize)
{
    int isNotEOF;
    int nextReadSize;
    char buf[BLOCKSIZE];
    Array *ret;

    isNotEOF = 1;
    if (!(ret = newArray(1, 1, sizeof(buf))))
        goto error1;
    do {
        nextReadSize = fread(buf, 1, BLOCKSIZE, fp);
        if (nextReadSize != sizeof(buf)) {
            buf[nextReadSize] = '\0';
            isNotEOF = 0;
        }
        if (!tryPushArray(&ret, buf))
            goto error2;
    } while(isNotEOF);
    if (outSize)
        *outSize = (ret->count - 1) * ret->elementSize + nextReadSize;
    return ret;
error2:;
    deleteArray(ret);
error1:;
    return NULL;
}

/*
 * converts a string of integers to an array of integers
 */
Array *
stringToIntArray(const char *str)
{
    Array *ret;
    int nextInt;
    const char *tmp1;
    const char *tmp2;

    if (!(ret = newArray(-1, -1, sizeof(int))))
        goto error1;
    tmp1 = str;
    while (1) {
        nextInt = strtol(tmp1, &tmp2, 10);
        if (tmp1 == tmp2)
            break;
        if (!(ret = pushArray(ret, &nextInt)))
            goto error2;
        tmp1 = tmp2;
    }
    return ret;
error2:;
    deleteArray(ret);
error1:;
    return NULL;
}

