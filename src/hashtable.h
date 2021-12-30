#ifndef ALIB_HASH_TABLE_H
#define ALIB_HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "./array.h"
#include "./vlarray.h"

typedef struct HashTable HashTable;
typedef uint32_t (*HashFunc)(const uint8_t *data, size_t n);

struct HashTable
{
    char isDirty;
    size_t nonEmptyBuckets;
    float maxLoadFactor;
    float loadFactor;
    VLArray *keys;
    VLArray *values;
    Array *records;
    Array *hashes;
    HashFunc hashFunc;
};

HashTable *newHashTable(HashFunc hashFunc, int capacity, float maxLoadFactor);
void deleteHashTable(HashTable *ht);
int insertHashTable(HashTable *ht, const uint8_t *key, size_t nKey,
    const uint8_t *value, size_t nValue);
int getHashTable(const HashTable *ht, const uint8_t *key,
    size_t nKey);
int growHashTable(HashTable *ht, size_t n);
float getLoadFactor(HashTable *ht);
void *getValueHashTable(const HashTable *ht, const uint8_t *key, size_t nKey);
uint32_t crc32(const uint8_t *data, size_t n);

#define getCountHashTable(HASH_TABLE_PTR) \
    (getCountVLArray((HASH_TABLE_PTR)->keys))

#define getBucketCountHashTable(HASH_TABLE_PTR) \
    (getCountArray((HASH_TABLE_PTR)->records))

#endif
