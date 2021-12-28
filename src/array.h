#ifndef ALIB_ARRAY_H
#define ALIB_ARRAY_H

/*
 * Aidan Bird 2021
 * 
 * Really simple dynamically sized array.
 *
 */ 

#include <stddef.h>
#include <stdint.h>

typedef struct Array Array;
typedef Array * (*ArrayRelocateFunc)();

void deleteArray(Array *array);
void clearArray(Array *array);
int removeAtArray(Array *array, void *outElement, size_t index);
int popArray(Array *array, void *outElement);
size_t arrayToString(Array *array, char **outStr);
void arrayToRaw(Array *array, size_t n);
int containsIndexArray(const Array *array, size_t index);
int removeRangeArray(Array *array, void *outElements, const size_t *range,
    size_t n);
int searchArray(const Array *array, const void *restrict element);
int removeContinuousRangeArray(Array *array, void *outElements, size_t index,
    size_t n);
Array *cloneArray(const Array *array);
Array *expandArray(Array *array, size_t blockCount);
Array *newArray(int blockSize, int capacity, size_t elementSize);
Array *pushArray(Array *array, const void *element);
Array *tryPushArray(Array **array, const void *element);
Array *insertArray(Array *array, const void *element, size_t index);
Array *growArray(Array *array, size_t n);
Array *forwardShiftRangeArray(Array *array, size_t index, size_t n);

/* TODO have a function that removes a continuous range of elements */ 

#define getCountArray(ARRAY_PTR) ((ARRAY_PTR)->count)
#define getElementArray(ARRAY_PTR, INDEX) \
    (((uint8_t *)(ARRAY_PTR)->first) + (ARRAY_PTR)->elementSize * (INDEX))
#define sizeofArray(ARRAY_PTR) ((ARRAY_PTR)->count * (ARRAY_PTR)->elementSize)
#define lastIndexArray(ARRAY_PTR) ((ARRAY_PTR)->count - 1)
#define getFirstArray(ARRAY_PTR) (getElementArray((ARRAY_PTR), 0))
#define isEmptyArray(ARRAY_PTR) (!(ARRAY_PTR)->count)
#define getLastArray(ARRAY_PTR) \
    (getElementArray(ARRAY_PTR, lastIndexArray(ARRAY_PTR)))
#define isIndexValidArray(ARRAY_PTR, INDEX) \
    ((INDEX) >= 0 && (INDEX) < (ARRAY_PTR)->count)

/*
 * ARRAY DETAILS AND FIELDS
 *
 * count = the number of elements in the array.
 *
 * capacity = the number of elements that can be added before the array is 
 * automatically resized.
 *
 * blockSize = the number of elements that will be added to the capacity when 
 * the array is resized.
 *
 * elementSize = the actual size of each element in bytes.
 *
 * first = a pointer to the first element in the array.
 * 
 * array elements should be accessed using the getElementArray() macro
 *
 * array functions that return an array pointer require that all subsequent 
 * calls these functions use the returned array pointer since the array pointer
 * in the parameter list could be resized or relocated.
 *
 * EXAMPLE
 *
 * Array *arr;
 * Array *tmp;
 *
 * tmp = arrayFunc(arr);
 * if (!tmp)
 *   // handle error
 * arr = tmp;
 */ 

struct Array
{
    size_t count;
    size_t capacity;
    size_t blockSize;
    size_t elementSize;
    void *first;
};

#endif
