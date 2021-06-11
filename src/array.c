/* Aidan Bird 2021 */ 
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "array.h"

#define DEFAULT_CAPACITY 32
#define DEFAULT_BLOCK_SIZE 32

/*
 * Array constructor
 * if capacity < 0, then the default capacity is used
 * if blockSize < 0, then the default blockSize is used
 * if blockSize = 0, then the array cannot be resized.
 */ 
Array *
newArray(int blockSize, int capacity, size_t elementSize)
{
    Array *ret;

    /* 
     * the actual array starts right after 
     * the memory taken up by the Array struct
     */
    capacity = capacity < 0 ? DEFAULT_CAPACITY : capacity;
    blockSize = blockSize <= 0 ? DEFAULT_BLOCK_SIZE : blockSize;
    if (!(ret = malloc(sizeof(Array) + elementSize * capacity)))
        return NULL;
    ret->count = 0;
    ret->capacity = capacity;
    ret->blockSize = blockSize;
    ret->elementSize = elementSize;
    ret->first = (void *)ret + sizeof(Array);
    return ret;
}

/*
 * Increase the capacity of an Array object by (Array.blockSize * blockCount).
 * Returns NULL on error.
 */ 
Array *
expandArray(Array *array, size_t blockCount)
{
    Array *ret;

    if (!array->blockSize || !blockCount)
        return array;
    ret = array;
    ret = realloc(ret, sizeof(Array) + array->elementSize * (array->capacity 
        + array->blockSize * blockCount));
    if (!ret)
        return NULL;
    ret->first = (void *)ret + sizeof(Array);
    ret->capacity += ret->blockSize * blockCount;
    return ret;
}

/* XXX experimental */
Array *
tryPushArray(Array **array, const void *element)
{
    void *tmp;

    if (!(tmp = insertArray(*array, element, (*array)->count)))
        return NULL;
    *array = tmp;
    return tmp;
}

/*
 * Append an element to an Array object. If there is not enough space for 
 * the next element, 
 * Returns NULL if the element cannot be appended.
 *
 * pushing elements takes O(1) if there is enough capacity.
 * takes O(n) time otherwise.
 */ 
Array *
pushArray(Array *array, const void *element)
{
    return insertArray(array, element, array->count);
}

/*
 * Remove element at a specified index.
 * If outElement is not NULL, the removed element will be copied to outElement.
 *
 * takes O(1) time if index = array.count - 1 i.e., pop operation.
 * takes O(n) time otherwise.
 */ 
int 
removeAtArray(Array *array, void *outElement, size_t index)
{
    return removeContinuousRangeArray(array, outElement, index, 1);
}

/*
 * remove the last element from the array
 * If outElement is not NULL, the removed element will be copied to outElement.
 *
 * takes O(1) time
 */ 
int
popArray(Array *array, void *outElement)
{
    return removeAtArray(array, outElement, array->count - 1);
}

/*
 * remove all elements from the array
 */ 
void
clearArray(Array *array)
{
    array->count = 0;
}

/*
 * Array destructor
 */ 
void
deleteArray(Array *array)
{
    free(array);
}

/*
 * Interprets the array contents as a null-terminated string.
 * Converts the array into a null-terminated string and returns the length of 
 *  the string.
 * Sets the outStr pointer to point to the string.
 *
 * Returns a negative number if realloc fails to shrink the outStr; 
 *  the array struct will be clobbered
 *
 * takes O(n) time
 */ 
int
arrayToString(Array *array, char **outStr)
{
    // void *tmp;
    size_t len;
    int ret;

    /* XXX use strlen */ 
    len = strnlen(array->first, sizeofArray(array) - 1);
    ret = len;
    memmove(array, array->first, len);
    ((char *)array)[len] = '\0';
    // tmp = array;
    // if (!(tmp = realloc(tmp, len + 1)))
    //     ret = -1;
    // *outStr = tmp;
    *outStr = array;
    return ret;
}

/*
 * Make a copy of an array.
 * The capacity will be set to the number of elements in the array.
 *
 * takes O(n) time
 */ 
Array *
cloneArray(Array *array)
{
    Array *ret;
    size_t size;

    size = sizeof(Array) + array->elementSize * array->count;
    if (!(ret = malloc(size)))
        return NULL;
    memcpy(ret, array, size);
    ret->first = (void *)ret + sizeof(Array);
    ret->capacity = ret->count;
    return ret;
}

/*
 * check if an index is compatible with an array
 */ 
int
containsIndexArray(Array *array, size_t index)
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

/*
 * remove multiple elements (specified by range) from an array.
 * If outElements is not NULL, the removed elements will be copied to 
 * outElements.
 * n is the number of elements to be removed.
 * the range must contain no duplicates
 */ 
int
removeRangeArray(Array *array, void *outElements, const size_t *range,
    size_t n)
{
    size_t r[n];
    char tmp[sizeofArray(array)];
    size_t i;
    size_t j;

    memcpy(r, range, sizeof(size_t) * n);
    qsort(r, n, sizeof(size_t), sortCmpFunc);
    /* copy deleted elements to outElements */ 
    if (outElements) {
        for (int k = 0; k < n; k++) {
            memcpy(outElements + array->elementSize * k, 
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

/*
 * do a linear search from low to high indexes.
 * return the index of the first elements that matches element.
 * returns -1 if no matches are found.
 *
 * takes O(n) time.
 */ 
int
searchArray(Array *array, const void *element)
{
    for (int i = 0; i < array->count; i++) {
        if (!memcmp(getElementArray(array, i), element, array->elementSize))
            return i;
    }
    return -1;
}

/* 
 * make space for n more elements.
 * resizes the array if needed.
 * returns NULL on error.
 *
 * takes O(n) if the array is resized. 
 */ 
Array *
growArray(Array *array, size_t n)
{
    if (array->count + n >= array->capacity) {
        return !array->blockSize ? NULL :
            expandArray(array, 1 + n / array->blockSize);
    }
    return array;
}


/*
 * Insert an element in the array at index.
 * If element is null, an empty element is inserted.
 * Returns a pointer to the array.  The pointer may differ if the array size 
 * is expanded. All subsequent uses of the array should be made using 
 * the return value.
 * Returns NULL on error, and the array data is left untouched.
 *
 * this takes O(n) time if index != array.count i.e., non-push insertion.
 * appending elements takes O(1) if there is enough capacity
 */ 
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

/*
 * make space for n elements at index by shifting elements forward
 * returns NULL on error.
 *
 * this takes O(n) time.
 */ 
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
        memmove(start + n * array->elementSize, start, (array->count - index) 
            * array->elementSize);
    }
    array->count += n;
    return array;
}

/*
 * remove a continuous range of n elements.
 *
 * returns non-zero on error e.g., array is empty, n is zero, 
 * index + n out of bounds.
 *
 * This takes O(n) time for non-pop operations.
 * Pop operations take O(1) time.
 */ 
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

/* XXX experimental */
void
arrayToRaw(Array *array, size_t n)
{
    n = n > sizeofArray(array) ? sizeofArray(array) : n;
    memmove(array, array->first, n);
}
