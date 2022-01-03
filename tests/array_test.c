#include <stdlib.h>
#include <check.h>
#include "../src/array.h"
#include "../src/utils.h"

static void
testTryPushArray(Array *arr, const int *testset, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(tryPushArray(&arr, testset + i), 
            "tryPushArray() failed");
    }
}

static void
testGetElementArray(Array *arr, const int *testset, size_t n)
{
    ck_assert_msg(getCountArray(arr) == n, "element count mismatch");
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(*(const int *)getElementArray(arr, i) == testset[i],
            "element and testset differ");
    }
}

static void
testClearArray(Array *arr)
{
    clearArray(arr);
    ck_assert_msg(getCountArray(arr) == 0, "clearArray() failed");
}

// static void
// test array Read operations



static void
testPopArray(Array *arr)
{
}

START_TEST (test_array) {
    Array *arr;
    const int testingData[] = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5 };

    puts("testing newArray()");
    arr = newArray(-1, -1, sizeof(int));
    ck_assert_msg(arr, "newArray() returned null");
    puts("testing tryPushArray()");
    testTryPushArray(arr, testingData, LEN(testingData));
    puts("testing getElementArray()");
    testGetElementArray(arr, testingData, LEN(testingData));
    puts("testing clearArray()");
    testClearArray(arr);

    // puts("testing pop");
    // 
    // removeAtArray()
    // puts("testing delete");

    deleteArray(arr);
}
END_TEST

Suite *
array_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Array");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_array);
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
