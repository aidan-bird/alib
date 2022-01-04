#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <stdio.h>
#include "../src/hashtable.h"
#include "../src/utils.h"
#include "../src/hashing.h"

#define N_VALID_KEYS 1000

static const char *testKeysConstLen[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", 
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
};
static const char *testValuesConstLen[] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};
static const char *testKeysVarLen[] = {
    "a", "bb", "ccc"
};
static const char *testValuesVarLen[] = {
    "AAA", "BB", "C"
};
static const int _testValuesInt[] = {
    -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5 
};
static const int _testKeysInt[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};
static const int *testValuesInt[LEN(_testValuesInt)];
static const int *testKeysInt[LEN(_testKeysInt)];
static const float maxLoadFactor = 0.75;
static const float capacity = 10;
static size_t testKeyConstLenSizes[LEN(testKeysConstLen)];
static size_t testKeyVarLenSizes[LEN(testKeysVarLen)];
static size_t testValueVarLenSizes[LEN(testKeysVarLen)];

static void
setupTestValues()
{
    size_t tmp;

    /* setup testing data */
    tmp = strlen(testKeysConstLen[0]) + 1;
    for (size_t i = 0; i < LEN(testKeyConstLenSizes); i++)
        testKeyConstLenSizes[i] = tmp;
    for (size_t i = 0; i < LEN(testKeyVarLenSizes); i++)
        testKeyVarLenSizes[i] = strlen(testKeysVarLen[i]) + 1;
    for (size_t i = 0; i < LEN(testValueVarLenSizes); i++)
        testValueVarLenSizes[i] = strlen(testValuesVarLen[i]) + 1;
    for (size_t i = 0; i < LEN(testValuesInt); i++)
        testValuesInt[i] = _testValuesInt + i;
    for (size_t i = 0; i < LEN(testKeysInt); i++)
        testKeysInt[i] = _testKeysInt + i;
}

static HashTable *
spawnHashTable()
{
    HashTable *ht;

    ht = newHashTable(crc32, capacity, maxLoadFactor);
    ck_assert_msg(ht != NULL, "newHashTable() returned NULL");
    return ht;
}

static void
checkKVConstKVSize(const HashTable *ht, const void **expectedKeys,
    size_t expectedKeySize, const void **expectedValues,
    size_t expectedValueSize, size_t n)
{
    int kvIndex;
    const void *retrievedValue;
    size_t retrievedValueSize;

    for (size_t i = 0; i < n; i++) {
        /* get record */
        kvIndex = getHashTable(ht, expectedKeys[i], expectedKeySize);
        ck_assert_msg(kvIndex >=0,
            "expected key (#%d) is not mapped to any value", i);
        /* get value */
        retrievedValue = getValueHashTable(ht, expectedKeys[i],
            expectedKeySize);
        ck_assert_msg(retrievedValue != NULL,
            "getValueHashTable() returned NULL");
        /* get value size */
        retrievedValueSize = sizeOfElementVLArray(ht->values, kvIndex);
        /* check if value matches the expected value */
        ck_assert_msg(expectedValueSize == retrievedValueSize,
            "expected value size (#%d) differs", i);
        ck_assert_msg(!memcmp(expectedValues[i], retrievedValue,
            expectedValueSize), "expected value (#%d) differs", i);
    }
}

static void
checkKV(const HashTable *ht, const void **expectedKeys,
    const size_t *expectedKeySizes, const void **expectedValues,
    const size_t *expectedValueSizes, size_t n)
{
    int kvIndex;
    const void *retrievedValue;
    size_t retrievedValueSize;

    for (size_t i = 0; i < n; i++) {
        /* get record */
        kvIndex = getHashTable(ht, expectedKeys[i], expectedKeySizes[i]);
        ck_assert_msg(kvIndex >= 0,
            "expected key (#%d) is not mapped to any value", i);
        /* get value */
        retrievedValue = getValueHashTable(ht, expectedKeys[i],
            expectedKeySizes[i]);
        ck_assert_msg(retrievedValue != NULL,
            "getValueHashTable() returned NULL");
        /* get value size */
        retrievedValueSize = sizeOfElementVLArray(ht->values, kvIndex);
        /* check if value matches the expected value */
        ck_assert_msg(expectedValueSizes[i] == retrievedValueSize,
            "expected value size (#%d) differs", i);
        ck_assert_msg(!memcmp(expectedValues[i], retrievedValue,
            expectedValueSizes[i]), "expected value (#%d) differs", i);
    }
}

static HashTable *
insertHashTableStringConstLen()
{
    HashTable *ht;

    ht = spawnHashTable();
    for (size_t i = 0; i < LEN(testKeysConstLen); i++) {
        ck_assert_msg(insertHashTable(ht, testKeysConstLen[i],
            testKeyConstLenSizes[0], testValuesConstLen[i],
            testKeyConstLenSizes[0]) >= 0,
            "insertHashTable failed with const len str key value: %s, %s",
            testKeysConstLen[i], testValuesConstLen[i]);
    }
    return ht;
}

static HashTable *
insertHashTableStringVarLen()
{
    HashTable *ht;

    ht = spawnHashTable();
    for (size_t i = 0; i < LEN(testKeysVarLen); i++) {
        ck_assert_msg(insertHashTable(ht, testKeysVarLen[i],
            testKeyVarLenSizes[i], testValuesVarLen[i], 
            testValueVarLenSizes[i]) >= 0,
            "insertHashTable failed with var len str key value: %s, %s",
            testKeysVarLen[i], testValuesVarLen[i]);
    }
    return ht;
}

static HashTable *
insertHashTableInt()
{
    HashTable *ht;

    size_t tmp;
    uint32_t hash;

    ht = spawnHashTable();
    for (size_t i = 0; i < LEN(testKeysInt); i++) {
        ck_assert_msg(insertHashTable(ht, testKeysInt[i], sizeof(int),
            testValuesInt[i], sizeof(int)) >= 0,
            "insertHashTable failed with int key value: %d, %d",
            *testKeysInt[i], *testValuesInt[i]);
        hash = getHashHashTable(ht, i);
        tmp = getBucketIDHashTable(ht, hash);
    }
    return ht;
}

/* XXX */

static void
testHashTableInsert(HashTable *ht, size_t n)
{
    size_t len;
    char key[16];

    for (size_t i = 0; i < n; i++) {
        sprintf(key, "%u", i);
        len = strlen(key) + 1;
        // printf("Inserting (%s, %s)\n", key, key);
        ck_assert_msg(insertHashTable(ht, key, len, key, len) >= 0,
            "cannot insert (%s, %s) into hashtable", key, key);
    }
}

static void
testRetrieveValidKeys(HashTable *ht)
{
    const void *validKey;
    const void *expectedValue;
    const void *recordValue;
    size_t recordValueSize;
    size_t validKeySize;
    size_t expectedValueSize;
    int kvIndex;

    for (size_t i = 0; i < getCountHashTable(ht); i++) {
        /* fetch known KV */
        validKey = getElementVLArray(ht->keys, i);
        validKeySize = sizeOfElementVLArray(ht->keys, i);
        expectedValue = getElementVLArray(ht->values, i);
        expectedValueSize = sizeOfElementVLArray(ht->values, i);
        /* fetch value using valid key */
        kvIndex = getHashTable(ht, validKey, validKeySize);
        ck_assert_msg(kvIndex >= 0, "got invalid kvIndex");
        recordValue = getElementVLArray(ht->values, kvIndex);
        recordValueSize = sizeOfElementVLArray(ht->values, kvIndex);
        /* test fetched value against known value */
        ck_assert_msg(recordValueSize == expectedValueSize,
            "fetched value and known value sizes differ");
        ck_assert_msg(!memcmp(recordValue, expectedValue, expectedValueSize),
            "fetched value and known value differ");
    }
}

START_TEST(testNewHashTable_NullCheck) 
{
    HashTable *ht;

    ht = spawnHashTable();
    deleteHashTable(ht);
}
END_TEST

START_TEST(testNewHashTable_CheckFields)
{
    HashTable *ht;

    ht = spawnHashTable();
    /* check fields */
    ck_assert_msg(ht->loadFactor == 0, "LoadFactor is invalid");
    ck_assert_msg(ht->isDirty == 0, "isDirty is invalid");
    ck_assert_msg(ht->nonEmptyBuckets == 0, "nonEmptyBuckets is invalid");
    ck_assert_msg(ht->maxLoadFactor == maxLoadFactor,
        "maxLoadFactor is invalid");
    ck_assert_msg(ht->hashFunc == crc32, "hashFunc is invalid");
    /* check capacity */
    // ck_assert_msg(getCapacityArray(ht->records) == capacity,
    //     "capacity mismatch in records");
    // ck_assert_msg(getCapacityArray(ht->hashes) == capacity,
    //     "capacity mismatch in hashes");
    ck_assert_msg(getCapacityVLArray(ht->keys) == capacity,
        "capacity mismatch in keys");
    ck_assert_msg(getCapacityVLArray(ht->values) == capacity,
        "capacity mismatch in values");
    deleteHashTable(ht);
}
END_TEST

// START_TEST(testDeleteHashTable) 
// {
// }
// END_TEST

START_TEST(testInsertHashTable_stringConstLen)
{
    HashTable *ht;

    ht = insertHashTableStringConstLen();
    deleteHashTable(ht);
}
END_TEST

START_TEST(testInsertHashTable_stringVarLen)
{
    HashTable *ht;

    ht = insertHashTableStringVarLen();
    deleteHashTable(ht);
}
END_TEST

START_TEST(testInsertHashTable_Int)
{
    HashTable *ht;

    ht = insertHashTableInt();
    deleteHashTable(ht);
}
END_TEST

START_TEST(testGetHashTable_StringConstLen)
{
    HashTable *ht;

    ht = insertHashTableStringConstLen();
    checkKV(ht, testKeysConstLen, testKeyConstLenSizes, testValuesConstLen,
        testKeyConstLenSizes, LEN(testKeysConstLen));
    deleteHashTable(ht);
}
END_TEST

START_TEST(testGetHashTable_StringVarLen) 
{
    HashTable *ht;

    ht = insertHashTableStringVarLen();
    checkKV(ht, testKeysVarLen, testKeyVarLenSizes, testValuesVarLen,
        testValueVarLenSizes, LEN(testKeysVarLen));
    deleteHashTable(ht);
}
END_TEST

START_TEST(testGetHashTable_Int) 
{
    HashTable *ht;

    ht = insertHashTableInt();
    checkKVConstKVSize(ht, testKeysInt, sizeof(int), testValuesInt,
        sizeof(int), LEN(testKeysInt));
    deleteHashTable(ht);
}
END_TEST

Suite *
ht_suite()
{
    Suite *ret;
    TCase *tcCore;

    ret = suite_create("HashTable");
    tcCore = tcase_create("Core");
    tcase_add_test(tcCore, testNewHashTable_NullCheck);
    tcase_add_test(tcCore, testNewHashTable_CheckFields);
    tcase_add_test(tcCore, testInsertHashTable_stringConstLen);
    tcase_add_test(tcCore, testInsertHashTable_stringVarLen);
    tcase_add_test(tcCore, testInsertHashTable_Int);
    tcase_add_test(tcCore, testGetHashTable_StringConstLen);
    tcase_add_test(tcCore, testGetHashTable_StringVarLen);
    tcase_add_test(tcCore, testGetHashTable_Int);
    suite_add_tcase(ret, tcCore);
    return ret;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    setupTestValues();
    /* run tests */
    s = ht_suite();
    sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return !number_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
