/* Aidan Bird 2021 */ 
// #define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "array.h"

#define DEFAULT_CAPACITY 32
#define DEFAULT_BLOCK_SIZE 32
#define MAXWORD uint64_t

/*
 * REQUIRES
 * none
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * Constructs a new array.
 *
 * blockSize = the number of elements that will be added to the capacity when 
 * the array is resized.
 *
 * elementSize = the actual size of each element in bytes.
 *
 * capacity = the number of elements that can be added before the array is 
 * automatically resized.
 *
 * if capacity < 0, then the default capacity is used
 * if blockSize < 0, then the default blockSize is used
 * if blockSize = 0, then the array cannot be resized.
 *
 * returns null on error.
 */
Array *
newArray(int blockSize, int capacity, size_t elementSize)
{
    Array *ret;
    size_t vectorSize;

    /* 
     * the actual array starts right after 
     * the memory taken up by the Array struct
     */
    capacity = capacity < 0 ? DEFAULT_CAPACITY : capacity;
    blockSize = blockSize <= 0 ? DEFAULT_BLOCK_SIZE : blockSize;
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * Increase the capacity of an Array by (Array.blockSize * blockCount).
 * On success, this returns a pointer to the resized array. This value may 
 * differ from the array point argument, so the array pointer argument is not
 * valid after a successful call to this function.
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
    ret->first = (uint8_t *)ret + sizeof(Array);
    ret->capacity += ret->blockSize * blockCount;
    return ret;
}

/* XXX experimental */
Array *
tryPushArray(Array **array, const void *element)
{
    Array *tmp;

    if (!(tmp = insertArray(*array, element, (*array)->count)))
        return NULL;
    *array = tmp;
    return tmp;
}

/*
 * REQUIRES
 * array is valid
 * element is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * Append an element to an Array object. 
 * The array may be automatically resized.
 * Pushing elements takes O(1) if there is enough capacity.
 * Pushing takes O(n) time otherwise.
 * Returns NULL on error.
 * See expandArray() for details about how the return value should be 
 * handled.
 */
Array *
pushArray(Array *array, const void *element)
{
    return insertArray(array, element, array->count);
}

/*
 * REQUIRES
 * array is valid
 * index is valid
 *
 * MODIFIES
 * array
 * outElement
 *
 * EFFECTS
 * Remove the element at a specified index.
 * If outElement is not NULL, the removed element will be copied to outElement.
 * takes O(1) time if index = array.count - 1 i.e., pop operation.
 * takes O(n) time otherwise.
 * returns non-zero on error
 */
int 
removeAtArray(Array *array, void *outElement, size_t index)
{
    return removeContinuousRangeArray(array, outElement, index, 1);
}

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 * outElement
 *
 * EFFECTS
 * remove the last element from the array.
 * If outElement is not NULL, the removed element will be copied to outElement.
 * takes O(1) time
 * returns non-zero on error
 */
int
popArray(Array *array, void *outElement)
{
    return removeAtArray(array, outElement, array->count - 1);
}

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * remove all elements from the array
 * this does not change the array capacity.
 * takes O(1) time
 */
void
clearArray(Array *array)
{
    array->count = 0;
}

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * frees array
 */
void
deleteArray(Array *array)
{
    free(array);
}

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * outStr
 *
 * EFFECTS
 * Interprets the array contents as a null-terminated string.
 * The array must contain a null terminator.
 * Converts the array into a null-terminated string and returns the length of 
 *  the string.
 * Sets the outStr pointer to point to the string.
 * The array pointer argument and the array itself is not valid after the 
 * function ends.
 * takes O(n) time
 */
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * Make a copy of an array.
 * The capacity will be set to the number of elements in the array.
 * takes O(n) time
 */
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * returns non-zero if index cannot be used to index into array
 */
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 * outElements
 *
 * EFFECTS
 * Remove multiple elements specified by range from an array.
 * If outElements is not NULL, the removed elements will be copied to 
 * outElements.
 * n is the number of elements to be removed.
 * the range must contain no duplicates.
 */
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * do a linear search from low to high indexes.
 * return the index of the first elements that matches element.
 * element must be the same type as the elements in the array.
 * returns -1 if no matches are found.
 * takes O(n) time.
 */
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

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * make space for n more elements.
 * resizes the array if needed.
 * returns NULL on error.
 * takes O(n) if the array is resized. 
 * See expandArray() for details about how the return value should be 
 * handled.
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
 * REQUIRES
 * array is valid
 * index is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * Insert an element in the array at index.
 * If element is null, an empty element is inserted.
 * Returns a pointer to the array. 
 * The pointer may differ if the array is resized. 
 * All subsequent uses of the array should be made using the return value.
 * Returns NULL on error, and the array data is left untouched.
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
 * REQUIRES
 * array is valid
 * index is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * make space for n elements at index by shifting elements forward
 * returns NULL on error.
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
        memmove((uint8_t *)start + n * array->elementSize, start, (array->count - index) 
            * array->elementSize);
    }
    array->count += n;
    return array;
}

/*
 * REQUIRES
 * array is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * remove a continuous range of n elements.
 * returns non-zero on error e.g., array is empty, n is zero, index + n out 
 * of bounds.
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
