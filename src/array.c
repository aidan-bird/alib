/* Aidan Bird 2023 */ 

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "array.h"

#define MAXWORD uint64_t

Array *
newArray(int blockSize, int capacity, size_t elementSize)
{
    Array *ret;
    size_t vectorSize;

    /* 
     * the actual array starts right after 
     * the memory taken up by the Array struct
     */
    /* negative values indicate that default values should be used */
    capacity = capacity < 0 ? ARRAY_DEFAULT_CAPACITY : capacity;
    blockSize = blockSize <= 0 ? ARRAY_DEFAULT_BLOCK_SIZE : blockSize;
    vectorSize = sizeof(Array) + elementSize * capacity;
    if (!(ret = malloc(vectorSize)))
        return NULL;
    ret->count = 0;
    ret->capacity = capacity;
    ret->blockSize = blockSize;
    ret->elementSize = elementSize;
    ret->first = (uint8_t *)ret + sizeof(Array);
    return ret;
}

Array *
expandArray(Array *array, size_t blockCount)
{
    Array *ret;

    if (!array->blockSize || !blockCount)
        return array;
    /* resize the array */
    ret = array;
    ret = realloc(ret, sizeof(Array) + array->elementSize * (array->capacity 
        + array->blockSize * blockCount));
    if (!ret)
        return NULL;
    /* reconfigure the resized array's fields */
    ret->first = (uint8_t *)ret + sizeof(Array);
    ret->capacity += ret->blockSize * blockCount;
    return ret;
}

Array *
pushArray(Array *array, const void *element)
{
    return insertArray(array, element, array->count);
}

Array *
tryPushArray(Array **array, const void *element)
{
    Array *tmp;

    tmp = pushArray(*array, element);
    if (!tmp)
        return NULL;
    *array = tmp;
    return tmp;
}

int 
removeAtArray(Array *array, void *outElement, size_t index)
{
    return removeContinuousRangeArray(array, outElement, index, 1);
}

int
popArray(Array *array, void *outElement)
{
    return removeAtArray(array, outElement, array->count - 1);
}

void
clearArray(Array *array)
{
    array->count = 0;
}

void
deleteArray(Array *array)
{
    free(array);
}

size_t
arrayToString(Array *array, char **outStr)
{
    size_t ret;

    ret = strlen(array->first);
    memmove(array, array->first, ret);
    ((char *)array)[ret] = '\0';
    if (outStr)
        *outStr = (char *)array;
    return ret;
}

Array *
cloneArray(const Array *array)
{
    Array *ret;
    size_t vectorSize;

    vectorSize = sizeof(Array) + array->elementSize * array->count;
    if (!(ret = malloc(vectorSize)))
        return NULL;
    memcpy(ret, array, vectorSize);
    ret->first = (uint8_t *)ret + sizeof(Array);
    ret->capacity = ret->count;
    return ret;
}

int
containsIndexArray(const Array *array, size_t index)
{
    if (!array->count)
        return -1;
    return index < array->count ? 0 : -1;
}

static int
sortCmpFunc(const void *a, const void *b)
{
   return *(size_t*)a - *(size_t*)b;
}

int
removeRangeArray(Array *array, void *outElements, const size_t *range,
    size_t n)
{
    /* XXX gonna rewrite this one */
    /* XXX change this to assume that range is sorted */
    size_t r[n];
    char tmp[sizeofArray(array)];
    size_t i;
    size_t j;

    memcpy(r, range, sizeof(size_t) * n);
    qsort(r, n, sizeof(size_t), sortCmpFunc);
    /* copy deleted elements to outElements */ 
    if (outElements) {
        for (size_t k = 0; k < n; k++) {
            memcpy((uint8_t *)outElements + array->elementSize * k, 
                getElementArray(array, r[k]), array->elementSize);
        }
    }
    /* remove elements */ 
    i = 0;
    j = 0;
    for (; i < array->count; i++) {
        if (bsearch(&i, r, n, sizeof(size_t), sortCmpFunc))
            continue;
        memcpy(tmp + j * array->elementSize, getElementArray(array, i),
            array->elementSize);
        j++;
    }
    memcpy(getElementArray(array, 0), tmp, array->elementSize * j);
    array->count = j;
    return 0;
}

int
searchArray(const Array *array, const void *restrict element)
{
    MAXWORD tmpA;
    MAXWORD tmpB;
    
    if (array->elementSize <= sizeof(MAXWORD)) {
        tmpA = 0;
        memcpy(&tmpA, element, array->elementSize);
        for (size_t i = 0; i < array->count; i++) {
            tmpB = 0;
            memcpy(&tmpB, getElementArray(array, i), array->elementSize);
            if (tmpA == tmpB)
                return i;
        }
    } else {
        for (size_t i = 0; i < array->count; i++) {
            if (!memcmp(getElementArray(array, i), element, array->elementSize))
                return i;
        }
    }
    return -1;
}

Array *
growArray(Array *array, size_t n)
{
    if (array->count + n >= array->capacity) {
        return !array->blockSize ? NULL :
            expandArray(array, 1 + n / array->blockSize);
    }
    return array;
}

Array *
insertArray(Array *array, const void *element, size_t index)
{
    if (array->count) {
        if (index > array->count)
            return NULL;
        if (!(array = growArray(array, 1)))
            return NULL;
        if (element) {
            if (index == array->count) {
                /* push operation */ 
                memcpy(getElementArray(array, array->count), element,
                    array->elementSize);
            } else {
                /* insert operation */
                /* move elements upward to make space */ 
                if (!(array = forwardShiftRangeArray(array, index, 1)))
                    return NULL;
                /* copy the new element into the array */ 
                memcpy(getElementArray(array, index), element,
                    array->elementSize);
                return array;
            }
        }
    } else {
        /* adding 0th element to an empty array */ 
        if (index)
            return NULL;
        if (!(array = growArray(array, 1)))
            return NULL;
        if (element)
            memcpy(array->first, element, array->elementSize);
    }
    array->count++;
    return array;
}

Array *
forwardShiftRangeArray(Array *array, size_t index, size_t n)
{
    void *start;

    if (!n)
        return array;
    if (index > array->count)
        return NULL;
    if (!(array = growArray(array, n)))
        return NULL;
    if (index < array->count) {
        start = getElementArray(array, index);
        memmove((uint8_t *)start + n * array->elementSize, start, (array->count - index) 
            * array->elementSize);
    }
    array->count += n;
    return array;
}

int
removeContinuousRangeArray(Array *array, void *outElements, size_t index,
    size_t n)
{
    void *addrDeleted;

    if (!array->count || !n)
        return -1;
    addrDeleted = getElementArray(array, index);
    if (outElements)
        memcpy(outElements, addrDeleted, n * array->elementSize);
    if (index + n - 1 < array->count - 1) {
        memmove(addrDeleted, getElementArray(array, index + n),
            array->elementSize * (array->count - index - n));
        array->count -= n;
    } else if (index + n - 1 == array->count - 1) {
        /* pop operation */ 
        array->count = index + n - 1;
    } else {
        return -1;
    }
    return 0;
}

void
arrayToRaw(Array *array, size_t n)
{
    n = n > sizeofArray(array) ? sizeofArray(array) : n;
    memmove(array, array->first, n);
}

int
compareArray(const Array *array1, const Array *array2)
{
    /* check if array ptrs are the same */
    if (array1 == array2)
        return 0;
    /* compare fields */
    if (array1->blockSize != array2->blockSize
        || array1->elementSize != array2->elementSize
        // || array1->capacity != array2->capacity
        || array1->count != array2->count) {
        return -1;
    }
    /* compare contents */
    for (size_t i = 0; i < getCountArray(array1); i++) {
        if (memcmp(getElementArray(array1, i), getElementArray(array2, i),
            array1->elementSize)) {
            return -1;
        }
    }
    return 0;
}
