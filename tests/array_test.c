#include <stdlib.h>
#include <check.h>
#include "../src/array.h"
#include "../src/utils.h"

static const int testingData1[] = { -2, -1, 0, 1, 2, 3, 4, 5 };
static const char *testingStr1 = "My god it's full of mail!";

static Array *
_newArray(size_t elementSize)
{
    Array *ret;

    /* new array with defaults */
    ck_assert_msg((ret = newArray(-1, -1, elementSize)) != NULL,
        "newretay() failed");
    return ret;
}

static Array *
_tryPushArray(Array **array, const void *element)
{
    size_t oldCount;
    void *topElement;
    Array *ret;

    oldCount = getCountArray(*array);
    ret = tryPushArray(array, element);
    ck_assert_msg(ret != NULL, "tryPushArray() failed");
    /* check that the element count was incremented */
    ck_assert_msg(oldCount + 1 == getCountArray(*array),
        "element count was not incremented");
    /* check top element */
    topElement = getLastArray(ret);
    ck_assert_msg(!memcmp(element, topElement, ret->elementSize),
        "top element mismatch");
    return ret;
}

static Array *
newArrayWithInt(const int *ints, size_t n)
{
    Array *ret;

    ret = _newArray(sizeof(int));
    /* put all ints into ret */
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(_tryPushArray(&ret, ints + i) != NULL,
            "tryPushArray() failed with element %d (at index %ld)",
            ints[i], i);
    }
    return ret;
}

static Array *
newArrayWithChars(const char *str)
{
    Array *ret;
    size_t i = 0;

    ret = _newArray(sizeof(char));
    /* put all ints into ret */
    do {
        ck_assert_msg(_tryPushArray(&ret, str) != NULL,
            "tryPushArray() failed with element %c (at index %ld)", *str, i);
        i++;
    } while(*(str++));
    return ret;
}

START_TEST (test_newArray_sizes)
{
    Array *arr;
    /* various sizes */
    const size_t expectedSizes[] = { 
        sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(size_t),
        sizeof(void *), sizeof(Array),
    };
    /* check that the array element sizes match the expected sizes */
    for (size_t i = 0; i < LEN(expectedSizes); i++) {
        arr = _newArray(expectedSizes[i]);
        ck_assert_msg(arr->elementSize == expectedSizes[i],
            "expected size mismatch (size: %ld, index: %ld)", expectedSizes[i],
            i);
        deleteArray(arr);
    }
}
END_TEST

START_TEST (test_array_insert)
{
    Array *arr;
    int got;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    /* check that the inserted element match the testing data */
    for (size_t i = 0; i < LEN(testingData1); i++) {
        got = *(int *)getElementArray(arr, i);
        ck_assert_msg(got == testingData1[i],
            "expected value mismatch (expected %d, got %d, index %ld)",
            testingData1[i], got, i);
    }
    deleteArray(arr);
}
END_TEST

START_TEST (test_array_compare)
{
    Array *arr;
    Array *clone;
    int got;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    /* make clone */
    ck_assert_msg((clone = cloneArray(arr)) != NULL, "cloneArray() failed");
    /* check that the clone is valid */
    for (size_t i = 0; i < getCountArray(arr); i++) {
        got = *(int *)getElementArray(arr, i);
        ck_assert_msg(got == testingData1[i],
            "expected value mismatch (expected %d, got %d, index %ld)",
            testingData1[i], got, i);
    }
    deleteArray(arr);
    deleteArray(clone);
}

START_TEST (test_array_clone)
{
    Array *arr;
    Array *clone;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    /* make clone */
    ck_assert_msg((clone = cloneArray(arr)) != NULL, "cloneArray() failed");
    /* check that the clone is valid */
    ck_assert_msg((!compareArray(arr, clone)) && (arr != clone),
        "clone is not valid");
    deleteArray(arr);
    deleteArray(clone);
}
END_TEST

START_TEST (test_array_pop)
{
    Array *arr;
    int got;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    /* pop off and check all elements */    
    while (!isEmptyArray(arr)) {
        popArray(arr, &got);
        ck_assert_msg(got == testingData1[getCountArray(arr)],
            "element mismatch (expected %d, got %d)",
            testingData1[getCountArray(arr)], got);
    }
    deleteArray(arr);
}
END_TEST

START_TEST (test_array_ToRaw)
{
    Array *arr;
    int got;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    arrayToRaw(arr, sizeofArray(arr));
    ck_assert_msg(!memcmp(arr, testingData1, sizeof(testingData1)),
        "raw and testingData1 mismatch");
    free(arr);
}
END_TEST

START_TEST (test_array_clear)
{
    Array *arr;
    int got;

    arr = newArrayWithInt(testingData1, LEN(testingData1));
    clearArray(arr);
    ck_assert_msg(arr->count == 0, "array was not cleared");
    deleteArray(arr);
}
END_TEST

START_TEST (test_array_toString)
{
    Array *arr;
    char *result;
    size_t len;
    size_t resultLen;

    result = NULL;
    arr = newArrayWithChars(testingStr1);
    len = arrayToString(arr, &result);
    ck_assert_msg(result != NULL, "array failed to convert to string");
    resultLen = strlen(result);
    ck_assert_msg(resultLen == len, "result length and emitted length differ");
    ck_assert_msg(resultLen == strlen(testingStr1),
        "resul length and expected length differ");
    ck_assert_msg(!strcmp(result, testingStr1),
        "converted string and expected string differ");
    free(arr);
}
END_TEST

START_TEST (test_array_contains_index)
{
    Array *arr;
    size_t validIndexes[] = {0, 1, 2};

    arr = newArrayWithInt(testingData1, LEN(testingData1)); 
    /* check valid indexes */
    for (size_t i = 0; i < LEN(validIndexes); i++) {
        ck_assert_msg(!containsIndexArray(arr, validIndexes[i]),
            "sent valid index, and got non-zero but expected otherwise");
    }
    /* check invalid indexes */
    ck_assert_msg(containsIndexArray(arr, getCountArray(arr)),
            "sent invalid index, and got zero but expected otherwise");
    deleteArray(arr);
}
END_TEST

START_TEST (test_array_search_success)
{
    int searchResult;
    const int expected = LEN(testingData1) - 1;
    Array *arr;

    arr = newArrayWithInt(testingData1, LEN(testingData1)); 
    /* search for a (last) element in the testingData1 */
    searchResult = searchArray(arr, testingData1 + expected);
    ck_assert_msg(searchResult >= 0,
        "expected searchArray() to find the element, but it failed");
    ck_assert_msg(searchResult == expected,
        "wrong index (expected %d, got %d)", expected, searchResult);
    ck_assert_msg(!memcmp(getElementArray(arr, searchResult),
        getElementArray(arr, expected), arr->elementSize),
        "values at search result index and expected index differ");
    deleteArray(arr);
}
END_TEST

START_TEST (test_array_search_fail)
{
    int searchResult;
    int sum;
    Array *arr;

    /* generate a value that is not in the testing data */
    sum = 0;
    for (size_t i = 0; i < LEN(testingData1); i++)
        sum += testingData1[i];
    arr = newArrayWithInt(testingData1, LEN(testingData1)); 
    /* try to search of an element that is not in the array */
    searchResult = searchArray(arr, &sum);
    ck_assert_msg(searchResult < 0,
        "expected searchArray() to not find the element (%d)", sum);
    deleteArray(arr);
}
END_TEST

// START_TEST (test_array_remove_at)
// {
//     Array *arr;
// 
//     arr = newArrayWithInt(testingData1, LEN(testingData1)); 
// 
//     removeAtArray(arr, );
// 
//     deleteArray(arr);
// }
// END_TEST

Suite *
array_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Array");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_newArray_sizes);
    tcase_add_test(tc_core, test_array_insert);
    tcase_add_test(tc_core, test_array_compare);
    tcase_add_test(tc_core, test_array_clone);
    tcase_add_test(tc_core, test_array_pop);
    tcase_add_test(tc_core, test_array_ToRaw);
    tcase_add_test(tc_core, test_array_clear);
    tcase_add_test(tc_core, test_array_toString);
    tcase_add_test(tc_core, test_array_contains_index);
    tcase_add_test(tc_core, test_array_search_success);
    tcase_add_test(tc_core, test_array_search_fail);
    suite_add_tcase(s, tc_core);
    return s;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = array_suite();
    sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
