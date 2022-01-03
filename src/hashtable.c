#include <stdlib.h>
#include <string.h>
#ifdef ALIB_TESTING 
#include <check.h>
#endif

#include "./hashtable.h"
#include "./utils.h"

#define HASH_TABLE_DEFAULT_CAPACITY 32

static void deleteBucketsByHashes(Array *buckets, const uint32_t *hashes,
    size_t n);

/*
 * REQUIRES
 * hashFunc is a valid hash function
 *
 * EFFECTS
 * initializes a new hash table.
 */
HashTable *
newHashTable(HashFunc hashFunc, int capacity, float maxLoadFactor)
{
    HashTable *ret;
    const void *nullElement = NULL;

    if (capacity < 0)
        capacity = HASH_TABLE_DEFAULT_CAPACITY;
    if (!(ret = malloc(sizeof(HashTable))))
        goto error1;
    if (!(ret->keys = newVLArray(-1, capacity, -1)))
        goto error2;
    if (!(ret->values = newVLArray(-1, capacity, -1)))
        goto error3;
    if (!(ret->records = newArray(-1, capacity, sizeof(Array *))))
        goto error4;
    if (!(ret->hashes = newArray(-1, capacity, sizeof(uint32_t))))
        goto error5;
    /* NULL elements denote uninitialized buckets */
    for (size_t i = 0; i < capacity; i++) {
        if (!(tryPushArray(&ret->records, &nullElement)))
            goto error6;
    }
    ret->hashFunc = hashFunc;
    ret->loadFactor = 0.0;
    ret->nonEmptyBuckets = 0;
    ret->maxLoadFactor = maxLoadFactor;
    ret->isDirty = 0;
    return ret;
error6:;
    deleteArray(ret->hashes);
error5:;
    deleteArray(ret->records);
error4:;
    deleteVLArray(ret->values);
error3:;
    deleteVLArray(ret->keys);
error2:;
    free(ret);
error1:;
    return NULL;
}

static void
deleteBuckets(HashTable *ht)
{
    uint32_t hash;
    Array **bucketPtr;
    size_t bucketID;
    size_t deleteCount; 

    deleteCount = 0;
    for (size_t i = 0; (i < getCountArray(ht->hashes))
        && (deleteCount < ht->nonEmptyBuckets); i++) {
        /* delete all buckets */
        hash = getHashHashTable(ht, i);
        bucketID = getBucketIDHashTable(ht, hash);
        bucketPtr = getBucketPtrFromBucketIDHashTable(ht, bucketID);
        if (!*bucketPtr)
            continue;
        deleteArray(*bucketPtr);
        *bucketPtr = NULL;
        deleteCount++;
    }
#ifdef ALIB_TESTING 
    ck_assert_msg(deleteCount == ht->nonEmptyBuckets,
        "%ld buckets were not deleted", ht->nonEmptyBuckets - deleteCount);
#endif
    ht->nonEmptyBuckets = 0;
}

/*
 * MODIFIES
 * ht
 *
 * EFFECTS
 * deletes ht
 */
void
deleteHashTable(HashTable *ht) 
{
    if (!ht)
        return;
    deleteBuckets(ht);
    deleteArray(ht->records);
    deleteArray(ht->hashes);
    deleteVLArray(ht->keys);
    deleteVLArray(ht->values);
    free(ht);
}

/*
 * REQUIRES
 * key and value are nKey and nValue bytes long.
 * key has not already been inserted into ht.
 *
 * MODIFIES
 * ht
 *
 * EFFECTS
 * inserts a key value pair into the hash table.
 * may cause relocations (expensive).
 * returns non-zero on error.
 */
int
insertHashTable(HashTable *ht, const uint8_t *key, size_t nKey,
    const uint8_t *value, size_t nValue) 
{
    size_t hash;
    size_t kvIndex;
    size_t bucketID;
    Array **bucketPtr;

    /* hash the key */
    hash = ht->hashFunc(key, nKey);
    /* save the key */
    if (!(tryPushVLArray(&ht->keys, key, nKey)))
        goto error1;
    /* 
     * kvIndex indexes into both keys and values, since keys are index mapped 
     * to values
     */
    kvIndex = lastIndexVLArray(ht->keys);
    /* save the value */
    if (!(tryPushVLArray(&ht->values, value, nValue)))
        goto error1;
    /* resize if needed */
    if (getLoadFactor(ht) > ht->maxLoadFactor) {
        if (growHashTable(ht, ht->records->capacity))
            goto error1;
    }
    /* make new bucket if needed */
    bucketID = getBucketIDHashTable(ht, hash);
    bucketPtr = (Array **)getElementArray(ht->records, bucketID);
    if (!*bucketPtr) {
        if (!(*bucketPtr = newArray(1, 1, sizeof(size_t))))
            goto error1;
        ht->nonEmptyBuckets++;
    }
    /* push record into bucket */
    if (!(tryPushArray(bucketPtr, &kvIndex)))
        goto error1;
    /* save hash so it does not need to be recomputed every resize */
    if (!(tryPushArray(&ht->hashes, &hash)))
        goto error1;
    ht->isDirty = 1;
    return 0;
error1:;
    return 1;
}

/*
 * REQUIRES
 * key is nKey bytes long.
 *
 * EFFECTS
 * returns the index of the kv pair. 
 * returns negative on error
 */
int
getHashTable(const HashTable *ht, const uint8_t *key, size_t nKey)
{
    size_t bucketID;
    size_t testKeySize;
    size_t kvIndex;
    const Array *bucket;
    const void *testKey;

    bucketID = getBucketIDHashTable(ht, ht->hashFunc(key, nKey));
    bucket = *(Array **)getElementArray(ht->records, bucketID);
    if (!bucket)
        return -1;
    for (size_t i = 0; i < getCountArray(bucket); i++) {
        kvIndex = *(size_t *)getElementArray(bucket, i);
        testKeySize = sizeOfElementVLArray(ht->keys, kvIndex);
        if (testKeySize != nKey)
            continue;
        testKey = getElementVLArray(ht->keys, kvIndex);
        if (!memcmp(testKey, key, nKey))
            return kvIndex;
    }
    return -1;
}

/*
 * MODIFIES
 * ht
 *
 * EFFECTS
 * enlarges ht.
 * returns non-zero on error
 */
int
growHashTable(HashTable *ht, size_t n)
{
    size_t nonEmptyBuckets;
    size_t newCapacity;
    uint32_t hash;
    size_t newBucketID;
    Array *newBuckets;
    const void *nullElement;
    Array **bucketPtr;
    uint32_t hashesAdded[getCountArray(ht->hashes)];

    /* allocate more buckets */
    nonEmptyBuckets = 0;
    newCapacity = getBucketCountHashTable(ht) + n;
    nullElement = NULL;
    if (!(newBuckets = newArray(ht->records->blockSize, newCapacity, 
        sizeof(Array *))))
        goto error1;
    /* null elements denotes uninitialized bucket */
    for (size_t i = 0; i < newCapacity; i++) {
        if (!(tryPushArray(&newBuckets, &nullElement)))
            goto error2;
    }
    /* insert all keys into the new buckets */
    for (size_t i = 0; i < getCountArray(ht->hashes); i++) {
        /* get hash code and new bucketID */
        hash = getHashHashTable(ht, i);
        newBucketID = hash % newCapacity;
        /* initialize new bucket if needed */
        bucketPtr = (Array **)getElementArray(newBuckets, newBucketID);
        if (!*bucketPtr) {
            if (!(*bucketPtr = newArray(1, 1, sizeof(size_t))))
                goto error3;
            hashesAdded[nonEmptyBuckets] = hash;
            nonEmptyBuckets++;
        }
        /* put next record into bucket */
        if (!(tryPushArray(bucketPtr, &i)))
            goto error3;
    }
    /* delete old buckets */
    deleteBuckets(ht);
    deleteArray(ht->records);
    /* configure ht to use new buckets */
    ht->records = newBuckets;
    ht->isDirty = 1;
    ht->nonEmptyBuckets = nonEmptyBuckets;
    return 0;
error3:;
    /* free new buckets */
    deleteBucketsByHashes(newBuckets, hashesAdded, nonEmptyBuckets);
error2:;
    deleteArray(newBuckets);
error1:;
    return -1;
}

/* XXX untested */
static void
deleteBucketsByHashes(Array *buckets, const uint32_t *hashes, size_t n)
{
    size_t deletedBucketID;
    Array **deletedBucketPtr;

    for (size_t i = 0; i < n; i++) {
        /* get bucket by hash */
        deletedBucketID = hashes[i] % getCountArray(buckets);
        deletedBucketPtr = (Array **)getElementArray(buckets,
            deletedBucketID);
        /* delete bucket */
        if (!*deletedBucketPtr)
            continue;
        deleteArray(*deletedBucketPtr);
        *deletedBucketPtr = NULL;
    }
}

static void
cleanHashTable(HashTable *ht)
{
    if (!ht->isDirty)
        return;
    ht->loadFactor = getCountVLArray(ht->keys) / getCountArray(ht->records);
    ht->isDirty = 0;
}

float
getLoadFactor(HashTable *ht)
{
    cleanHashTable(ht);
    return ht->loadFactor;
}

/*
 * EFFECTS
 * returns the value associated with a key.
 * returns NULL on error
 */
void *
getValueHashTable(const HashTable *ht, const uint8_t *key, size_t nKey)
{
    int kvIndex;

    if ((kvIndex = getHashTable(ht, key, nKey)) < 0)
        return NULL;
    return getElementVLArray(ht->values, kvIndex);
}
