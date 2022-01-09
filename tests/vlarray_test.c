#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "../src/vlarray.h"
#include "../src/utils.h"

#define testData1_removed1 "bb"
#define testData1_removed_last "ccc"
#define testData1_removed_first ""

static const char *testData1[] = { 
    testData1_removed_first, "a", testData1_removed1, testData1_removed_last,
};
size_t testData1Sizes[LEN(testData1)];

const char *testData1Str = 
testData1_removed_first "a" testData1_removed1 testData1_removed_last;

size_t testData1MaxLen;
size_t testData1ExpectedSize;

static void
setupTestingData()
{
    testData1ExpectedSize = 0;
    for (size_t i = 0; i < LEN(testData1); i++) {
        testData1Sizes[i] = strlen(testData1[i]) + 1;
        testData1ExpectedSize += testData1Sizes[i];
    }
    testData1MaxLen = testData1Sizes[0];
    for (size_t i = 1; i < LEN(testData1Sizes); i++)
        testData1MaxLen = MAX(testData1MaxLen, testData1Sizes[i]);
}

static VLArray *
_newVLArray()
{
    VLArray *ret;

    ret = newVLArray(-1, -1, -1);
    ck_assert_msg(ret != NULL, "newVLArray() failed");
    return ret;
}

static VLArray *
newVLArrayWithTestData1()
{
    VLArray *arr;

    arr = _newVLArray();
    for (size_t i = 0; i < LEN(testData1); i++) {
        ck_assert_msg(tryPushVLArray(&arr, testData1[i], testData1Sizes[i]),
            "tryPushVLArray() failed at index %ld", i);
    }
    return arr;
}

static int
_removeAtVLArray(VLArray *arr, void *restrict outElement, size_t i)
{
    char tmp1[sizeOfElementVLArray(arr, i)];
    char tmp2[sizeOfElementVLArray(arr, i)];

    /* save a copy of the element to check if it matches the outElement */
    memcpy(tmp1, getElementVLArray(arr, i), sizeof(tmp1));
    ck_assert_msg(!removeAtVLArray(arr, tmp2, i), "removeAtVLArray() failed");
    ck_assert_msg(!memcmp(tmp1, tmp2, sizeof(tmp1)), "outElement differs");
    if (outElement)
        memcpy(outElement, tmp1, sizeof(tmp1));
    return 0;
}

static void
_popVLArray(VLArray *arr, void *outElement)
{
    size_t elementSize = sizeOfElementVLArray(arr, lastIndexVLArray(arr));
    char lastElement[elementSize];
    char deleted[elementSize];

    memcpy(lastElement, peekVLArray(arr), elementSize);
    ck_assert_msg(!popVLArray(arr, deleted), "popVLArray() failed");
    ck_assert_msg(!memcmp(lastElement, deleted, elementSize),
        "outElement differs");
    if (outElement)
        memcpy(outElement, lastElement, elementSize);
}

START_TEST (test_vlarray_new)
{
    deleteVLArray(_newVLArray());
}
END_TEST

START_TEST (test_vlarray_push)
{
    deleteVLArray(newVLArrayWithTestData1());
}
END_TEST

START_TEST (test_vlarray_remove_middle)
{
    const size_t idx = 2;
    VLArray *arr;

    arr = newVLArrayWithTestData1();
    char tmp[sizeOfElementVLArray(arr, idx)];
    /* remove middle element */
    _removeAtVLArray(arr, tmp, idx);
    ck_assert_msg(!strcmp(tmp, testData1_removed1), "outElement differs");
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_remove_last)
{
    size_t idx;
    VLArray *arr;

    arr = newVLArrayWithTestData1();
    idx = getCountVLArray(arr) - 1;
    char tmp[sizeOfElementVLArray(arr, idx)];
    /* remove last element */
    _removeAtVLArray(arr, tmp, idx);
    ck_assert_msg(!strcmp(tmp, testData1_removed_last), "outElement differs");
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_remove_first)
{
    VLArray *arr;

    arr = newVLArrayWithTestData1();
    char tmp[sizeOfElementVLArray(arr, 0)];
    /* remove last element */
    _removeAtVLArray(arr, tmp, 0);
    ck_assert_msg(!strcmp(tmp, testData1_removed_first), "outElement differs");
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_pop)
{
    VLArray *arr;
    char expected[testData1MaxLen + 1];
    char popped[testData1MaxLen + 1];

    arr = newVLArrayWithTestData1();
    /* pop all off and compare */
    for (size_t i = 0; i < LEN(testData1); i++) {
        strcpy(expected, testData1[LEN(testData1) - 1 - i]);
        _popVLArray(arr, popped);
        ck_assert_msg(!strcmp(expected, popped), "popped value differs");
    }
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_clear)
{
    VLArray *arr;

    arr = newVLArrayWithTestData1();
    clearVLArray(arr);
    ck_assert_msg(!getCountVLArray(arr), "array was not cleared");
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_toString)
{
    VLArray *arr;
    char *str;

    arr = newVLArrayWithTestData1();
    str = toStringVLArray(arr);
    ck_assert_msg(!strcmp(str, testData1Str),
        "result does not match expected value");
    free(str);
    deleteVLArray(arr);
}
END_TEST

START_TEST (test_vlarray_get_size)
{
    size_t s;
    VLArray *arr;

    arr = newVLArrayWithTestData1();
    s = getSizeVLArray(arr);
    ck_assert_msg(testData1ExpectedSize == s, "expected size: %ld, got: %ld",
        testData1ExpectedSize, s);
    deleteVLArray(arr);
}
END_TEST

Suite *
array_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("VLArray");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_vlarray_new);
    tcase_add_test(tc_core, test_vlarray_push);
    tcase_add_test(tc_core, test_vlarray_remove_first);
    tcase_add_test(tc_core, test_vlarray_remove_middle);
    tcase_add_test(tc_core, test_vlarray_remove_last);
    tcase_add_test(tc_core, test_vlarray_pop);
    tcase_add_test(tc_core, test_vlarray_clear);
    tcase_add_test(tc_core, test_vlarray_toString);
    tcase_add_test(tc_core, test_vlarray_get_size);
    suite_add_tcase(s, tc_core);
    return s;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    setupTestingData();
    s = array_suite();
    sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
