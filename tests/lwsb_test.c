#include <stdlib.h>
#include <string.h>

#include <check.h>

#include "../src/lw_string_builder.h"
#include "../src/utils.h"

static LWStringBuilder *newLWSB();
static void setupTestingData();

/* testing data 1 (only strings)*/
static const char *testingData1[] = {
    "", "a", "bb", "ccc", "dddd",
};
static const char *testingData1Expect = "abbcccdddd";
static size_t testingData1Sizes[LEN(testingData1)];

/* testing data 2 (only chars)*/
static const char testingData2[] = {
    'a', 'b', 'c', '\0', 'd', 'a',
};
static const char *testingData2Expect = "abcda";

/* testing data 3 (strings + chars)*/
static const char *_testingData31[] = {
    "", "1", "foobar",
};
static const char _testingData32[] = {
    'a', '\0', 'b',
};
static const char *testingData3Expect = "a1bfoobar";
static const char *testingData3[LEN(_testingData31) + LEN(_testingData32)];
static size_t testingData3Sizes[LEN(_testingData31) + LEN(_testingData32)];

/* HELPER FUNCS */

static LWStringBuilder *
newLWSB()
{
    LWStringBuilder *ret;

    ck_assert_msg((ret = newLWStringBuilder()) != NULL,
        "newLWStringBuilder() returned NULL");
    return ret;
}

static void
setupTestingData()
{
    /* setup testingData1 */
    for (size_t i = 0; i < LEN(testingData1); i++)
        testingData1Sizes[i] = strlen(testingData1[i]);
    /* setup testingData3 */ 
    for (size_t i = 0, j = 0, k = 0; i < LEN(testingData3); i++) {
        if (i & 1) {
            testingData3[i] = _testingData31[j];
            testingData3Sizes[i] = strlen(_testingData31[j]);
            j++;
        } else {
            testingData3[i] = &_testingData32[k];
            testingData3Sizes[i] = 1;
            k++;
        }
    }
}

static LWStringBuilder *
newLWStringBuilderWithStrings1(const char **elements, size_t n)
{
    LWStringBuilder *ret;

    ret = newLWSB();
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(!LWStringBuilderPushStr(ret, elements[i]),
            "LWStringBuilderPushStr() failed");
    }
    return ret;
}

static LWStringBuilder *
newLWStringBuilderWithStrings2(const char **elements, const size_t *sizes,
    size_t n)
{
    LWStringBuilder *ret;

    ret = newLWSB();
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(!LWStringBuilderPushNStr(ret, elements[i], sizes[i]),
            "LWStringBuilderPushNStr() failed");
    }
    return ret;
}

static LWStringBuilder *
newLWStringBuilderWithChars(const char *elements, size_t n)
{
    LWStringBuilder *ret;

    ret = newLWSB();
    for (size_t i = 0; i < n; i++) {
        ck_assert_msg(!LWStringBuilderPushChar(ret, elements[i]),
            "LWStringBuilderPushChar() failed");
    }
    return ret;
}

static LWStringBuilder *
newLWStringBuilderMixed(const char **elements, const size_t *sizes, size_t n)
{
    LWStringBuilder *ret;

    ret = newLWSB();
    for (size_t i = 0; i < n; i++) {
        if (!(sizes[i] - 1)) {
            ck_assert_msg(!LWStringBuilderPushChar(ret, *(elements[i])),
                "LWStringBuilderPushChar() failed");
        } else {
            ck_assert_msg(!LWStringBuilderPushNStr(ret, elements[i], sizes[i]),
                "LWStringBuilderPushNStr() failed");
        }
    }
    return ret;
}

/* TESTS */
START_TEST(testLWSB_newLWStringBuilder)
{
    LWStringBuilder *sb;

    sb = newLWSB();
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_1a)
{
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithStrings1(testingData1, LEN(testingData1));
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_1b)
{
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithStrings2(testingData1, testingData1Sizes,
        LEN(testingData1));
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_2)
{
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithChars(testingData2, LEN(testingData2));
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_3)
{
    LWStringBuilder *sb;

    sb = newLWStringBuilderMixed(testingData3, testingData3Sizes,
        LEN(testingData3));
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_1a_construct)
{
    size_t resultLen;
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithStrings1(testingData1, LEN(testingData1));
    result = LWStringBuilderToString(sb, &resultLen);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData1Expect),
        "result and expected value differ");
    ck_assert_msg(strlen(testingData1Expect) == resultLen,
        "expected value length and outLen differ");
    ck_assert_msg(strlen(result) == resultLen,
        "result length and outLen differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_1a_construct_no_outLen)
{
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithStrings1(testingData1, LEN(testingData1));
    result = LWStringBuilderToString(sb, NULL);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData1Expect),
        "result and expected value differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_1b_construct)
{
    size_t resultLen;
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithStrings2(testingData1, testingData1Sizes,
        LEN(testingData1));
    result = LWStringBuilderToString(sb, &resultLen);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData1Expect),
        "result and expected value differ");
    ck_assert_msg(strlen(testingData1Expect) == resultLen,
        "expected value length and outLen differ");
    ck_assert_msg(strlen(result) == resultLen,
        "result length and outLen differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_2_construct_no_outLen)
{
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderWithChars(testingData2, LEN(testingData2));
    result = LWStringBuilderToString(sb, NULL);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData2Expect),
        "result and expected value differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_3_construct)
{
    size_t resultLen;
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderMixed(testingData3, testingData3Sizes,
        LEN(testingData3));
    result = LWStringBuilderToString(sb, &resultLen);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData3Expect),
        "result and expected value differ");
    ck_assert_msg(strlen(testingData3Expect) == resultLen,
        "expected value length and outLen differ");
    ck_assert_msg(strlen(result) == resultLen,
        "result length and outLen differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

START_TEST(testLWSB_testing_data_3_construct_no_outLen)
{
    char *result;
    LWStringBuilder *sb;

    sb = newLWStringBuilderMixed(testingData3, testingData3Sizes,
        LEN(testingData3));
    result = LWStringBuilderToString(sb, NULL);
    ck_assert_msg(result != NULL, "LWStringBuilderToString() failed");
    ck_assert_msg(!strcmp(result, testingData3Expect),
        "result and expected value differ");
    free(result);
    deleteLWStringBuilder(sb);
}
END_TEST

// START_TEST(testLWSB_writeLWStringBuilder) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderToString) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderGetSize) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderPushStr) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderPushChar) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderPushNStr) { } END_TEST
// START_TEST(testLWSB_deleteLWStringBuilder) { } END_TEST
// START_TEST(testLWSB_LWStringBuilderPrintInplace) { } END_TEST

Suite *
ht_suite()
{
    Suite *ret;
    TCase *tcCore;

    ret = suite_create("lw_string_builder");
    tcCore = tcase_create("Core");
    tcase_add_test(tcCore, testLWSB_newLWStringBuilder);
    tcase_add_test(tcCore, testLWSB_testing_data_1a);
    tcase_add_test(tcCore, testLWSB_testing_data_1b);
    tcase_add_test(tcCore, testLWSB_testing_data_2);
    tcase_add_test(tcCore, testLWSB_testing_data_3);
    tcase_add_test(tcCore, testLWSB_testing_data_1a_construct);
    tcase_add_test(tcCore, testLWSB_testing_data_1a_construct_no_outLen);
    tcase_add_test(tcCore, testLWSB_testing_data_1b_construct);
    tcase_add_test(tcCore, testLWSB_testing_data_2_construct_no_outLen);
    tcase_add_test(tcCore, testLWSB_testing_data_3_construct);
    tcase_add_test(tcCore, testLWSB_testing_data_3_construct_no_outLen);
    suite_add_tcase(ret, tcCore);
    return ret;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    setupTestingData();
    /* run tests */
    s = ht_suite();
    sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return !number_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
