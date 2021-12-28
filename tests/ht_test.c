#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "../src/hashtable.h"

START_TEST (new_HashTable) 
{
    HashTable *ht;

    ht = newHashTable(crc32, 10, 0.75);
    if (!ht)
        ck_abort_msg("newHashTable returned null");
}
END_TEST

START_TEST (insert_HashTable) 
{
    int iret;
    const char *k = "my key";
    const char *v = "my value";
    HashTable *ht;

    ht = newHashTable(crc32, 10);
    iret = insertHashTable(ht, k, strlen(k) + 1, v, strlen(v) + 1);
}
END_TEST

START_TEST (delete_HashTable) 
{
    HashTable *ht;

    ht = newHashTable(crc32, 10);
    if (!ht)
        ck_abort_msg("newHashTable returned null");
    deleteHashTable(ht);
}
END_TEST

START_TEST (get_HashTable) 
{
    int iret;
    const char *k = "my key";
    const char *v = "my value";
    const char *vv;
    const char *kk;
    HashTable *ht;
    ValueRecord *rec;

    ht = newHashTable(crc32, 10);
    iret = insertHashTable(ht, k, strlen(k) + 1, v, strlen(v) + 1);
    rec = getHashTable(ht, k, strlen(k) + 1);
    vv = getElementVLArray(ht->values, rec->valueIndex);
    kk = getElementVLArray(ht->keys, rec->keyIndex);
    puts(vv);
    puts(kk);
    deleteHashTable(ht);
}
END_TEST

// START_TEST (test_crc32)
// {
// }
// END_TEST

Suite *
array_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("HashTable");
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, new_HashTable);
    tcase_add_test(tc_core, delete_HashTable);
    tcase_add_test(tc_core, insert_HashTable);
    tcase_add_test(tc_core, get_HashTable);
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
