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

#define ARRAY_DEFAULT_CAPACITY 32
#define ARRAY_DEFAULT_BLOCK_SIZE 32

typedef struct Array Array;
typedef Array * (*ArrayRelocateFunc)();

/**
 * Deletes an array.
 * @param array pointer to array to delete
 */
void deleteArray(Array *array);

/**
 * Clears all elements in an array.
 * O(1) complexity
 * @param array pointer to array to clear.
 */
void clearArray(Array *array);

/**
 * Delete element in array at index.
 * O(1) complexity if index = array.count - 1 i.e., pop operation.
 * O(n) complexity otherwise.
 * @param array array to delete element from
 * @param outElement if not set to NULL, the deleted element content will be written to the location pointed to
 * @param index index of element to delete
 * @return non-zero if an error occurred
 */
int removeAtArray(Array *array, void *outElement, size_t index);

/**
 * delete the last element in an array.
 * O(1) complexity.
 * @param array array to delete element from
 * @param outElement if not set to NULL, the deleted element content will be written to the location pointed to
 * @return non-zero if an error occurred
 */
int popArray(Array *array, void *outElement);

/**
 * Interprets the array contents as a null-terminated string.
 * The array must contain a null terminator i.e., the last element is 0.
 * The array pointer is not valid after this function finishes.
 * O(n) complexity
 * @param array array to interpret as a string.
 * @param outStr Sets the outStr pointer to point to the string.
 * @return the converted-string length.
 */
size_t arrayToString(Array *array, char **outStr);

/**
 * Converts array into a raw sequence of n elements.
 * The array cannot be used in array functions & macros after this function 
 * because the array metadata is deleted.
 * @param array the array to convert into a raw sequence of n elements.
 * @param n the number of elements
 */
void arrayToRaw(Array *array, size_t n);

/**
 * check if an index is valid for use in an array.
 * @param array pointer to array to test
 * @param index the index to test
 * @return non-zero if index cannot be used to index into array
 */
int containsIndexArray(const Array *array, size_t index);

/**
 * remove a range of elements
 * @param array pointer to the array to delete elements from
 * @param outElements a pointer to a location that can store n consecutive elements, or NULL if no elements should be copied
 * @param range an array of indexes to remove
 * @param n number of elements in the range
 * @return non-zero if an error occurred.
 */
int removeRangeArray(Array *array, void *outElements, const size_t *range,
    size_t n);

/**
 * linear search an array for an element
 * O(n) complexity
 * @param array the pointer to the array to search
 * @param element pointer to the element to match against
 * @return the index of the element, or negative if no elements are found
 * TODO: change the function to take in a pointer to a size_t index, and write 
 * the location there if needed. the return value will just indicate if the
 * element was found, not the index.
 * TODO: add a way to bsearch the array assuming that it is sorted; the 
 * function can take in a comparison function for traversing the array based on
 * the order
 */
int searchArray(const Array *array, const void *restrict element);

/**
 * remove a continuous range of n elements.
 * O(n) complexity for non-pop operations.
 * O(1) complexity for pop operations.
 * @param array the array to delete elements from
 * @param outElements a pointer to a location that can store at least deleted n
 * elements, or NULL if no elements should be saved.
 * @param index index of first deleted element
 * @param n number of elements to delete.
 * @return non-zero if an error occurred
 * TODO: consider a contiguous range of n elements that ends at the last
 * element as a pop operation.
 */
int removeContinuousRangeArray(Array *array, void *outElements, size_t index,
    size_t n);

/**
 * duplicate an array
 * @param array the array to clone
 * @return pointer to the cloned array, or NULL if an error occurred.
 */
Array *cloneArray(const Array *array);

/**
 * expand the capacity of an array.
 * @param array the array to expand.
 * @param blockCount the number of blocks to expand the array by
 * @return a pointer to the expanded array.
 * All subsequent operations should use the returned value to access the array.
 * NULL will be returned if there is an error.
 */
Array *expandArray(Array *array, size_t blockCount);

/**
 * Create a new array.
 * @param blockSize the number of elements in a block
 * @param capacity initial block count
 * @param elementSize the size of each element
 * @return a pointer to the new array, or NULL if an error occurred.
 */
Array *newArray(int blockSize, int capacity, size_t elementSize);

/**
 * append elements to the end of an array.
 * O(1) complexity if there is enough capacity.
 * O(n) complexity otherwise.
 * @param array the array to append an element to
 * @param element the element to append
 * @reutrn a pointer to the array, all subsequent array operations should use
 * this pointer. NULL is returned if there was an error.
 */
Array *pushArray(Array *array, const void *element);

/**
 * attempt to push an element onto an array.
 * O(1) complexity if there is enough capacity.
 * O(n) complexity otherwise.
 * @param array pointer to an array pointer that is used to access the array.
 * The array pointer will be modified after this operation finishes.
 * @param element the element to push
 * @return returns NULL if an error occurred.
 */
Array *tryPushArray(Array **array, const void *element);

/**
 * insert an element at index.
 * O(1) complexity for push operation if there is enough capacity.
 * O(n) complexity if index < array.count - 1
 * @param array the array to insert into.
 * @param element the element to insert
 * @param index the location in the array to insert into.
 * @return If successful, the array parameter is invalid and all uses of the 
 * array are to be done using the return value; NULL otherwise.
 */
Array *insertArray(Array *array, const void *element, size_t index);

/**
 * make space for n more elements; a resize operation is only run if needed.
 * @param array pointer to the arary to expand.
 * @param n the number of elements more that the array may contain.
 * @return If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value; NULL if an error occurred.
 */
Array *growArray(Array *array, size_t n);

/**
 * Make space for n elements at index by shifting elements forward.
 * O(n) complexity
 * @param array the array to shift elements forward in
 * @param index the starting index of the elements to shift forward
 * @param n the number of elements to shift
 * @return If successful, the array parameter is invalid and all uses of the array
 * are to be done using the return value; NULL if an error occurred.
 */
Array *forwardShiftRangeArray(Array *array, size_t index, size_t n);

/**
 * compare all elements in an array against another array.
 * @param array1 array to compare against
 * @param array2 array to compare against
 * @returns non-zero if the arrays differ.
 */
int compareArray(const Array *array1, const Array *array2);

/* TODO have a function that removes a continuous range of elements */ 

/**
 * get the number of elements in an array 
 */
#define getCountArray(ARRAY_PTR) ((ARRAY_PTR)->count)

/**
 * get a pointer to an element at index
 */
#define getElementArray(ARRAY_PTR, INDEX) \
    (((uint8_t *)(ARRAY_PTR)->first) + (ARRAY_PTR)->elementSize * (INDEX))

/**
 * get the size of the array elements
 */
#define sizeofArray(ARRAY_PTR) ((ARRAY_PTR)->count * (ARRAY_PTR)->elementSize)

/**
 * get the index of the last element
 */
#define lastIndexArray(ARRAY_PTR) ((ARRAY_PTR)->count - 1)

/**
 * get a pointer to the first element
 */
#define getFirstArray(ARRAY_PTR) (getElementArray((ARRAY_PTR), 0))

/**
 * returns non-zero if the array is empty
 */
#define isEmptyArray(ARRAY_PTR) (!(ARRAY_PTR)->count)

/**
 * get a pointer to the last element
 */
#define getLastArray(ARRAY_PTR) \
    (getElementArray(ARRAY_PTR, lastIndexArray(ARRAY_PTR)))

/**
 * check if an index is valid
 */
#define isIndexValidArray(ARRAY_PTR, INDEX) \
    ((INDEX) >= 0 && (INDEX) < (ARRAY_PTR)->count)

/**
 * get the current array capacity
 */
#define getCapacityArray(ARRAY_PTR) ((ARRAY_PTR)->capacity)

/**
 * get the size of each element
 */
#define getElementSizeArray(ARRAY_PTR) ((ARRAY_PTR)->elementSize)

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
