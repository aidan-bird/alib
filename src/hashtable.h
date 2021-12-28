#ifndef ALIB_HASH_TABLE_H
#define ALIB_HASH_TABLE_H

#include <stddef.h>
#include <stdint.h>

#include "./array.h"
#include "./vlarray.h"

typedef struct HashTable HashTable;
typedef struct ValueRecord ValueRecord;
typedef size_t (*HashFunc)(const uint8_t *data, size_t n);

struct HashTable
{
    char isDirty;
    size_t nonEmptyBuckets;
    float maxLoadFactor;
    float loadFactor;
    VLArray *keys;
    VLArray *values;
    Array *records;
    HashFunc hashFunc;
};

struct ValueRecord
{
    size_t keyIndex;
    size_t hashCode;
    size_t valueIndex;
};

HashTable *newHashTable(HashFunc hashFunc, int capacity, float maxLoadFactor);
void deleteHashTable(HashTable *ht);
int insertHashTable(HashTable *ht, const uint8_t *key, size_t nKey,
    const uint8_t *value, size_t nValue);
const ValueRecord *getHashTable(const HashTable *ht, const uint8_t *key,
    size_t nKey);
int growHashTable(HashTable *ht, size_t n);
float getLoadFactor(HashTable *ht);

uint32_t crc32(const uint8_t *data, size_t n);

#define getCountHashTable(HASH_TABLE_PTR) \
    (getCountVLArray((HASH_TABLE_PTR)->keys))

#define getBucketCountHashTable(HASH_TABLE_PTR) \
    (getCountArray((HASH_TABLE_PTR)->records))

#endif
