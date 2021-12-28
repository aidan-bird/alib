#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "../src/hashtable.h"
#include "../src/utils.h"

int
main(void)
{
    int iret;
    const char *k[] = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", 
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
    };
    const char *v[] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
    const char *vv;
    const char *kk;
    HashTable *ht;
    ValueRecord *rec;

    ht = newHashTable(crc32, 10, 0.75);
    for (size_t i = 0; i < LEN(k); i++) {
        iret = insertHashTable(ht, k[i], 2, v[i], 2);
        if (iret)
            puts("errors detected");
    }
    for (size_t i = 0; i < LEN(v); i++) {
        rec = getHashTable(ht, k[i], 2);
        vv = getElementVLArray(ht->values, rec->valueIndex);
        kk = getElementVLArray(ht->keys, rec->keyIndex);
        puts(vv);
        puts(kk);
    }
    deleteHashTable(ht);
}
