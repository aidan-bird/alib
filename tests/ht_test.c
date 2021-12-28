#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <stdio.h>
#include "../src/hashtable.h"
#include "../src/utils.h"

#define N_VALID_KEYS 1000

void
testHashTableInsert(HashTable *ht, size_t n)
{
    size_t len;
    char key[16];

    for (size_t i = 0; i < n; i++) {
        sprintf(key, "%u", i);
        len = strlen(key) + 1;
        // printf("Inserting (%s, %s)\n", key, key);
        ck_assert_msg(insertHashTable(ht, key, len, key, len) == 0,
            "cannot insert (%s, %s) into hashtable", key, key);
    }
}

void
testRetrieveValidKeys(HashTable *ht)
{
    const void *validKey;
    const void *expectedValue;
    const ValueRecord *record;
    const void *recordValue;
    size_t recordValueSize;
    size_t validKeySize;
    size_t expectedValueSize;

    for (size_t i = 0; i < getCountHashTable(ht); i++) {
        /* fetch known KV */
        validKey = getElementVLArray(ht->keys, i);
        validKeySize = sizeOfElementVLArray(ht->keys, i);
        expectedValue = getElementVLArray(ht->values, i);
        expectedValueSize = sizeOfElementVLArray(ht->values, i);
        /* fetch value using valid key */
        record = getHashTable(ht, validKey, validKeySize);
        ck_assert_msg(record, "expected record, got NULL");
        recordValue = getElementVLArray(ht->values, record->valueIndex);
        recordValueSize = sizeOfElementVLArray(ht->values, record->valueIndex);
        /* test fetched value against known value */
        ck_assert_msg(recordValueSize == expectedValueSize,
            "fetched value and known value sizes differ");
        ck_assert_msg(!memcmp(recordValue, expectedValue, expectedValueSize),
            "fetched value and known value differ");
    }
}

START_TEST (test_HashTable)
{
    const size_t n = N_VALID_KEYS;
    char key[16];
    size_t nKey;
    size_t nValue;
    int r;
    const char *kk;
    const char *vv;
    const ValueRecord *rec;
    size_t len;
    const int invalidKeys[] = { -1, (N_VALID_KEYS + 1) };
    HashTable *ht;

    /* make new hash table */
    puts("Making new HashTable");
    ht = newHashTable(crc32, -1, 0.75);
    ck_assert_msg(ht != NULL, "newHashTable() returned NULL");
    /* test inserting KVs */
    printf("Inserting %u unique KVs", n);
    testHashTableInsert(ht, n);
    /* check if all of them were inserted */
    ck_assert_msg(getCountHashTable(ht) == n, "KV count mismatch");
    /* try retrieving all KVs */
    puts("Retrieving all KVs");
    testRetrieveValidKeys(ht);
    /* try invalid keys */
    puts("Trying invalid keys");
    for (size_t i = 0; i < LEN(invalidKeys); i++) {
        ck_assert_msg(!getHashTable(ht, invalidKeys + i, sizeof(int)),
            "expected NULL by using invalid key (%d)", invalidKeys[i]);
    }
    puts("hashtable test complete");
}
END_TEST

Suite *
ht_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("HashTable");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_HashTable);
    suite_add_tcase(s, tc_core);
    return s;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = ht_suite();
    sr = srunner_create(s);
    srunner_set_fork_status (sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return !number_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
