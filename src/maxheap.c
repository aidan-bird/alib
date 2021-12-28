#ifdef debug

#include <stdlib.h>

#include "./maxheap.h"

MaxHeap *
newMaxHeap(int blockSize, int capacity, size_t elementSize, MaxHeapCmpFunc cmp)
{
    MaxHeap *ret;

    if (!(ret = malloc(sizeof(MaxHeap))))
        goto error1;
    if (!(ret->raw = newArray(blockSize, capacity, elementSize)))
        goto error2;
    ret->cmp = cmp;
    return ret;
error2:;
    free(ret);
error1:;
    return NULL;
}

int
insertMaxHeap(MaxHeap *heap, const void *element)
{

    return 0;
}

void
deleteMaxHeap(MaxHeap *heap)
{
    if (!heap)
        return;
    deleteArray(heap->raw);
    free(heap);
}

#endif
