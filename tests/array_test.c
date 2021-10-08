#include <stdlib.h>
#include <check.h>
#include "../src/array.h"

START_TEST (test_array_new) {
    Array *arr;

    arr = newArray(-1, -1, sizeof(int));
    if (arr)
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
    tcase_add_test(tc_core, test_array_new);
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
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
