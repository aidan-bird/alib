/* Aidan Bird 2021 */ 

#include <stdlib.h>
#include <string.h>

#include "vlarray.h"
#include "utils.h"

#define VLARRAY_DEFAULT_FRAME_SIZE 64
#define VLARRAY_DEFAULT_CAPACITY_SIZE 32

#define arrayFunc(ARRAY_PTR, ARRAY_FUNC, ERROR_LABEL) \
{ \
    Array *arrayfunc_tmp_; \
    if (!(arrayfunc_tmp_ = (ARRAY_FUNC))) \
        goto ERROR_LABEL; \
    ARRAY_PTR = arrayfunc_tmp_; \
}

/*
 * REQUIRES
 * none
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * constructs a new VLArray.
 *
 * framesize = element sizes will be counted as the number of frames that they 
 * take up. This parameter determines how large each frame is.
 *
 * (blocksize & capacity) see array constructor in array.c
 *
 * returns null on error
 */
VLArray *
newVLArray(int blockSize, int capacity, int frameSize)
{
    VLArray *ret;

    frameSize = frameSize <= 0 ? VLARRAY_DEFAULT_FRAME_SIZE : frameSize;
    capacity = capacity <= 0 ? VLARRAY_DEFAULT_CAPACITY_SIZE : capacity;
    if (!(ret = malloc(sizeof(VLArray))))
        goto error1;
    if (!(ret->offsets = newArray(blockSize, capacity, sizeof(int))))
        goto error2;
    if (!(ret->data = newArray(!blockSize ? -1 : blockSize, capacity, 
        frameSize))) {
        goto error3;
    }
    if (!(ret->sizes = newArray(blockSize, capacity, sizeof(size_t))))
        goto error4;
    ret->isDirty = 1;
    return ret;
error4:;
    deleteArray(ret->data);
error3:;
    deleteArray(ret->offsets);
error2:;
    free(ret);
error1:;
    return NULL;
}

/*
 * REQUIRES
 * arr is valid
 *
 * MODIFIES
 * arr
 *
 * EFFECTS
 * delets arr
 *
 */
void
deleteVLArray(VLArray *arr)
{
    deleteArray(arr->offsets);
    deleteArray(arr->data);
    deleteArray(arr->sizes);
    free(arr);
}

/*
 * REQUIRES
 * arr is valid
 *
 * MODIFIES
 * arr
 *
 * EFFECTS
 * push an element to the end of the array.
 *
 * if nextElement is NULL, this will only reserve space.
 *
 * takes O(n) time.
 *
 * returns null on error.
 *
 * returns a pointer to the arr. all uses of arr should be done using the 
 * return value.
 */
VLArray *
pushVLArray(VLArray *arr, const void *nextElement, size_t elementSize)
{
    return insertVLArray(arr, nextElement, elementSize, arr->offsets->count);
}

/* XXX experimental */
VLArray *
tryPushVLArray(VLArray **arr, const void *nextElement, size_t elementSize)
{
    VLArray *tmp;

    tmp = pushVLArray(*arr, nextElement, elementSize);
    if (!tmp)
        return NULL;
    *arr = tmp;
    return tmp;
}

/*
 * REQUIRES
 * arr is valid
 *
 * MODIFIES
 * arr
 *
 * EFFECTS
 * insert an element into array at index
 *
 * see pushVLArray for errors
 *
 * takes O(n) time
 */
VLArray *
insertVLArray(VLArray *arr, const void *nextElement, size_t elementSize,
    size_t index)
{
    int nextFrames;
    int frameStartIndex;
    int offsetRecord;

    nextFrames = 1 + elementSize / arr->data->elementSize;
    if (!arr->offsets->count) {
        /* push the first element */ 
        if (index)
            return NULL;
        /* push new offset record */  
        offsetRecord = 0;
        arrayFunc(arr->offsets, pushArray(arr->offsets, &offsetRecord),
            error1);
        /* make space for first element */ 
        arrayFunc(arr->data, forwardShiftRangeArray(arr->data, 0, nextFrames),
            error1);
        /* push element data */ 
        if (nextElement)
            memcpy(arr->data->first, nextElement, elementSize);
        arrayFunc(arr->sizes, pushArray(arr->sizes, &elementSize), error1);
    } else {
        if (index > arr->offsets->count)
            return NULL;
        if (index == arr->offsets->count) {
            /* push operation */ 
            frameStartIndex = arr->data->count;
            /* make space for new element frames */ 
            arrayFunc(arr->data, forwardShiftRangeArray(arr->data,
                frameStartIndex, nextFrames), error1);
            /* register new element */ 
            if (nextElement)
                memcpy(getElementArray(arr->data, frameStartIndex),
                    nextElement, elementSize);
            /* push new offset record */  
            arrayFunc(arr->offsets, pushArray(arr->offsets, &frameStartIndex),
                error1);
            arrayFunc(arr->sizes, pushArray(arr->sizes, &elementSize), error1);
        } else {
            /* insert operation */ 
            frameStartIndex = *((int *)getElementArray(arr->offsets, index));
            /* make space for new element frames */ 
            arrayFunc(arr->data, forwardShiftRangeArray(arr->data,
                frameStartIndex, nextFrames), error1);
            /* register new element */ 
            if (nextElement)
                memcpy(getElementArray(arr->data, frameStartIndex),
                    nextElement, elementSize);
            /* update offset records */ 
            arrayFunc(arr->offsets, insertArray(arr->offsets, &frameStartIndex,
                index + 1), error1);
            for (int i = index + 1; i < arr->offsets->count; i++)
                *((int *)getElementArray(arr->offsets, i)) += nextFrames;
            arrayFunc(arr->sizes, insertArray(arr->sizes, &elementSize, index),
                error1);
        }
    }
    arr->isDirty = 1;
    return arr;
error1:;
    return NULL;
}

/*
 * remove an element at index
 *
 * takes O(n) time
 */ 
int
removeAtVLArray(VLArray *arr, void *outElement, size_t index)
{
    int offsetDeleted;
    int offsetNext;
    int frameDiff;
    void *addrDeleted;

    if (!arr->offsets->count || index >= arr->offsets->count)
        return -1;
    addrDeleted = getElementVLArray(arr, index);
    /* copy deleted elements to outElement */ 
    if (outElement)
        memcpy(outElement, addrDeleted, sizeOfElementVLArray(arr, index));
    if (!index && arr->offsets->count == 1) {
        /* special casse: there is only one element in the vla */ 
        clearVLArray(arr);
        return 0;
    }
    /* remove the element */ 
    offsetDeleted = getOffsetVLArray(arr, index);
    offsetNext = index == arr->offsets->count - 1 ? arr->data->count : getOffsetVLArray(arr, index + 1);
    frameDiff = offsetNext - offsetDeleted;
    removeContinuousRangeArray(arr->data, NULL, offsetDeleted, frameDiff);
    removeAtArray(arr->sizes, NULL, index);
    removeAtArray(arr->offsets, NULL, index);
    /* update the offset array */ 
    for (int i = index; i < arr->offsets->count; i++)
        getOffsetVLArray(arr, i) -= frameDiff;
    arr->isDirty = 1;
    return 0;
}

/*
 * remove the last element in the array 
 *
 * takes O(n) time
 */ 
int
popVLArray(VLArray *arr, void *outElement)
{
    return removeAtVLArray(arr, outElement, arr->offsets->count - 1);
}

/*
 * remove all elements in the array 
 *
 * takes O(1) time
 */ 
void
clearVLArray(VLArray *arr)
{
    clearArray(arr->offsets);
    clearArray(arr->data);
    clearArray(arr->sizes);
    arr->isDirty = 1;
}

void
printVLArray(const VLArray *arr)
{
    for (size_t i = 0; i < getCountVLArray(arr); i++)
        printBinary(getElementVLArray(arr, i), sizeOfElementVLArray(arr, i));
}

void
updateTotalSizeVLArray(VLArray *arr)
{
    arr->totalSize = 0;
    for (size_t i = 0; i < getCountVLArray(arr); i++)
        arr->totalSize += sizeOfElementVLArray(arr, i);
}

void
makeCleanVLArray(VLArray *arr)
{
    if (!arr->isDirty)
        return;
    updateTotalSizeVLArray(arr);
    arr->isDirty = 0;
}

size_t
getSizeVLArray(VLArray *arr)
{
    makeCleanVLArray(arr);
    return arr->totalSize;
}

char *
toStringVLArray(VLArray *arr)
{
    char *ret;
    size_t totalSize;
    size_t lastOffset;

    totalSize = getSizeVLArray(arr) - getCountVLArray(arr) + 1;
    ret = malloc(totalSize);
    if (!ret)
        goto error1;
    lastOffset = 0;
    for (size_t i = 0; i < getCountVLArray(arr); i++) {
        memcpy(ret + lastOffset, getElementVLArray(arr, i),
            sizeOfElementVLArray(arr, i) - 1);
        lastOffset += (!sizeOfElementVLArray(arr, i) ?
            1 : sizeOfElementVLArray(arr, i)) - 1;
    }
    ret[totalSize - 1] = '\0';
    return ret;
error1:;
    return NULL;
}
