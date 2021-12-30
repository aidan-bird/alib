#ifndef VLARRAY_H
#define VLARRAY_H

/*
 * Aidan Bird 2021
 *
 * An array-like structure that can have 
 * variable length elements e.g., strings.
 */ 

#include <stddef.h>

#include "array.h"

#define getElementVLArray(VLARRAY_PTR, INDEX) \
    (getElementArray((VLARRAY_PTR)->data, \
    (*((int *)getElementArray((VLARRAY_PTR)->offsets, INDEX)))))
#define getCountVLArray(VLARRAY_PTR) ((VLARRAY_PTR)->offsets->count)
#define sizeOfElementVLArray(VLARRAY_PTR, INDEX) \
    (*((size_t *)getElementArray((VLARRAY_PTR)->sizes, INDEX)))
#define getOffsetVLArray(VLARRAY_PTR, INDEX) \
    (*((int *)getElementArray((VLARRAY_PTR)->offsets, INDEX)))
#define isEmptyVLArray(VLARRAY_PTR) \
    (!(getCountVLArray(VLARRAY_PTR)))
#define isFullVLArray(VLARRAY_PTR) \
    ((getCountVLArray(VLARRAY_PTR)) >= ((VLARRAY_PTR)->offsets->capacity))
#define lastIndexVLArray(VLARRAY_PTR) \
    (getCountVLArray(VLARRAY_PTR) - 1)
#define peekVLArray(VLARRAY_PTR) \
    (getElementVLArray(VLARRAY_PTR, (lastIndexVLArray(VLARRAY_PTR))))
#define getCapacityVLArray(VLARRAY_PTR) ((VLARRAY_PTR)->offsets->capacity)

typedef struct VLArray VLArray;

VLArray *newVLArray(int blockSize, int capacity, int frameSize);
void deleteVLArray(VLArray *arr);
VLArray *pushVLArray(VLArray *arr, const void *nextElement,
    size_t elementSize);
int removeAtVLArray(VLArray *arr, void *outElement, size_t index);
int popVLArray(VLArray *arr, void *outElement);
void clearVLArray(VLArray *VLArray);
VLArray *insertVLArray(VLArray *arr, const void *nextElement, 
    size_t elementSize, size_t index);
void printVLArray(const VLArray *arr);
VLArray *tryPushVLArray(VLArray **arr, const void *nextElement,
    size_t elementSize);
size_t getSizeVLArray(VLArray *arr);
char *toStringVLArray(VLArray *arr);

/*
 * VLARRAY DETAILS AND FIELDS
 *
 * offsets = an array of index offsets that are used for indexing into the 
 * array.
 *
 * data = the array elements are stored here, and the offsets array is used to
 * index into this array.
 *
 * sizes = stores the size of each element
 *
 * For example, the elements in a variable length could be:
 * [0] = "my first string"
 * [1] = <a file pointer>
 * [2] = "my 2nd string"
 * [3] = <a very large string>
 * [4] = <some binary data>
 *
 * the getCountVLArray() macro returns the number of elements in the VLArray
 *
 * All vlarray indexing should use the getElementVLArray() macro
 *
 * VLArray functions that return a pointer to a VLArray have similar function 
 * and restrictions as the array functions. See array.h
 */ 

struct VLArray
{
    Array *offsets;
    Array *data;
    Array *sizes;
    int isDirty;
    size_t totalSize;
};

#endif
