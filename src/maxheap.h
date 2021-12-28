#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include <stddef.h>

#include "./array.h"

typedef int (*MaxHeapCmpFunc)(size_t na, const void *a,
    size_t nb, const void *b);

typedef struct MaxHeap MaxHeap;

struct MaxHeap
{
    Array *raw;
    MaxHeapCmpFunc cmp;
};

MaxHeap *newMaxHeap(int blockSize, int capacity, size_t elementSize,
    MaxHeapCmpFunc cmp);
void deleteMaxHeap(MaxHeap *heap);
MaxHeap *newMaxHeap(int blockSize, int capacity, size_t elementSize,
    MaxHeapCmpFunc cmp);

#endif
