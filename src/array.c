/* Aidan Bird 2021 */ 

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "array.h"

#define MAXWORD uint64_t

/*
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
 * If capacity < 0, then the default capacity is used.
 * If blockSize < 0, then the default blockSize is used.
 * If blockSize = 0, then the array cannot be resized.
 *
 * Returns null on error.
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

/*
 * MODIFIES
 * array
 *
 * EFFECTS
 * Increase the capacity of an Array by (Array.blockSize * blockCount).
 * On success, this returns a pointer to the resized array. This value may 
 * differ from the array point argument, so the array pointer argument is not
 * valid after a successful call to this function.
 * Returns NULL on error.
 * Does nothing if either blockCount or blockSize are zero.
 */
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

/*
 * MODIFIES
 * array
 *
 * EFFECTS
 * Append an element to an Array object. 
 * The array may be automatically resized (expensive).
 * Pushing elements takes O(1) if there is enough capacity, and O(n) otherwise.
 * Returns NULL on error.
 * If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value.
 */
Array *
pushArray(Array *array, const void *element)
{
    return insertArray(array, element, array->count);
}

/*
 * MODIFIES
 * array and *array
 *
 * EFFECTS
 * Pushes an element onto the array, and if successful, replace the array
 * pointer with a new pointer returned by pushArray().
 * Returns NULL on error.
 */
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

/*
 * REQUIRES
 * Index is valid.
 * If outElement is not null, it should point to a buffer that is at least 
 * elementSize bytes long.
 *
 * MODIFIES
 * array
 * outElement
 *
 * EFFECTS
 * Remove the element at a specified index.
 * If outElement is not NULL, the removed element will be copied to outElement.
 * Takes O(1) time if index = array.count - 1 i.e., pop operation.
 * Takes O(n) time otherwise.
 * Returns non-zero on error.
 */
int 
removeAtArray(Array *array, void *outElement, size_t index)
{
    return removeContinuousRangeArray(array, outElement, index, 1);
}

/*
 * REQUIRES
 * If outElement is not null, it should point to a buffer that is at least 
 * elementSize bytes long.
 *
 * MODIFIES
 * array
 * outElement
 *
 * EFFECTS
 * Remove the last element from the array.
 * If outElement is not NULL, the removed element will be copied to outElement.
 * Takes O(1) time.
 * Returns non-zero on error.
 */
int
popArray(Array *array, void *outElement)
{
    return removeAtArray(array, outElement, array->count - 1);
}

/*
 * MODIFIES
 * array
 *
 * EFFECTS
 * Remove all elements from the array
 * This does not change the array capacity.
 * Takes O(1) time.
 */
void
clearArray(Array *array)
{
    array->count = 0;
}

/*
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
 * MODIFIES
 * outStr
 *
 * EFFECTS
 * Interprets the array contents as a null-terminated string.
 * The array must contain a null terminator.
 * Converts the array into a null-terminated string, and returns the length of 
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
 * EFFECTS
 * Returns non-zero if index cannot be used to index into array.
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
 * MODIFIES
 * array
 * outElements
 *
 * EFFECTS
 * Remove multiple elements specified by a sequence of indexes from an array.
 * If outElements is not NULL, the removed elements will be copied to 
 * outElements.
 * n is the number of elements to be removed.
 * The range sequence must contain no duplicates.
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
 * EFFECTS
 * Do a linear search from low to high indexes.
 * Return the index of the first elements that matches element.
 * Element must be the same type and size as the elements in the array.
 * Returns negative if no matches are found.
 * Takes O(n) time.
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
 * MODIFIES
 * array
 *
 * EFFECTS
 * Make space for n more elements.
 * Resizes the array if needed (expensive).
 * Returns NULL on error.
 * Takes O(n) if the array is resized. 
 * If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value.
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
 * index is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * Insert an element in the array at index.
 * If element is null, an empty element is inserted.
 * Returns NULL on error, and the array data is left untouched.
 * Takes O(n) time if index < array.count - 1 i.e., non-push insertion.
 * appending elements takes O(1) if there is enough capacity.
 * If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value.
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
 * index is valid
 *
 * MODIFIES
 * array
 *
 * EFFECTS
 * Make space for n elements at index by shifting elements forward.
 * Returns NULL on error.
 * Takes O(n) time.
 * If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value.
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
 * MODIFIES
 * array
 *
 * EFFECTS
 * Remove a continuous range of n elements.
 * Returns non-zero on error e.g., array is empty, n is zero, index + n out 
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

/*
 * MODIFIES
 * array
 *
 * EFFECTS
 * Converts array into a raw sequence of n elements.
 * The array cannot be used in array functions & macros after this function.
 */
void
arrayToRaw(Array *array, size_t n)
{
    n = n > sizeofArray(array) ? sizeofArray(array) : n;
    memmove(array, array->first, n);
}

/*
 * EFFECTS
 * Returns non-zero if the arrays differ.
 */
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
