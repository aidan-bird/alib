// #ifdef debug
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "./lw_string_builder.h"
#include "./hashtable.h"
#include "./hashing.h"

typedef struct LWSBRecord LWSBRecord;

struct LWSBRecord
{
    size_t kvIndex;
};

struct LWStringBuilder
{
    Array *records;
    HashTable *strs;
    size_t size;
    int isDirty;
};

static size_t writeSome(LWStringBuilder *sb, char *buf, size_t n);
static void writeAll(LWStringBuilder *sb, char *buf);

LWStringBuilder *
newLWStringBuilder()
{
    LWStringBuilder *ret;
    if (!(ret = malloc(sizeof(LWStringBuilder)))) 
        goto error1;
    if (!(ret->records = newArray(-1, -1, sizeof(LWSBRecord))))
        goto error2;
    if (!(ret->strs = newHashTable(crc32, -1, 0.75)))
        goto error3;
    ret->size = 0;
    ret->isDirty = 0;
    return ret;
// error4:;
//     deleteHashTable(ret->strs);
error3:;
    deleteArray(ret->records);
error2:;
    free(ret);
error1:;
    return NULL;
}

static void
writeAll(LWStringBuilder *sb, char *buf)
{
    size_t kvIndex;
    size_t nextStrSize;
    size_t nextStrLen;
    const char *nextStr;

    for (size_t i = 0; i < getCountArray(sb->records); i++) {
        /* get kv details */
        kvIndex = ((LWSBRecord *)getElementArray(sb->records, i))->kvIndex;
        nextStr = (const char *)getKeyHashTable(sb->strs, kvIndex);
        nextStrSize = getSizeofKeyHashTable(sb->strs, kvIndex);
        /* check if the string is a string or just one char */
        switch (nextStrSize) {
            case 0:
                /* the next str is empty; ignore it */
                break;
            case 1:
                /* 
                 * the next str is actually just one char.
                 * if it is NULL, ignore it
                 */
                if (!*nextStr)
                    continue;
                /* the string is not null; write it to buf */
                *buf = *nextStr;
                buf++;
                break;
            default:
                /* 
                 * the next str is a null terminated str.
                 * write the string to buf.
                 */
                nextStrLen = nextStrSize - 1;
                memcpy(buf, nextStr, nextStrLen);
                buf += nextStrLen;
                break;
        }
    }
    /* add null terminator */
    *buf = '\0';
}

static size_t
writeSome(LWStringBuilder *sb, char *buf, size_t n)
{
    size_t ret;
    size_t kvIndex;
    size_t nextStrSize;
    size_t nextStrLen;
    size_t nextWriteSize;
    const char *nextStr;

    ret = 0;
    for (size_t i = 0; (i < getCountArray(sb->records)) && (ret < n); i++) {
        /* get kv details */
        kvIndex = ((LWSBRecord *)getElementArray(sb->records, i))->kvIndex;
        nextStr = (const char *)getKeyHashTable(sb->strs, kvIndex);
        nextStrSize = getSizeofKeyHashTable(sb->strs, kvIndex);
        /* check if the string is a string or just one char */
        if (nextStrSize == 1) {
            /* 
             * the next str is actually just one char.
             * if it is NULL, ignore it
             */
            if (!*nextStr)
                continue;
            /* the string is not null; write it to buf */
            *buf = *nextStr;
            ret++;
        } else {
            /* the next str is a null terminated str */
            nextStrLen = nextStrSize - 1;
            /* if needed, limit write Length so that it fits in buf */
            nextWriteSize = ret + nextStrLen > n ? n - ret : nextStrLen;
            /* write the string to buf */
            memcpy(buf, nextStr, nextWriteSize);
            ret += nextWriteSize;
            buf += nextWriteSize;
        }
        /* add null terminator */
        *buf = '\0';
    }
    return ret;
}

/*
 * REQUIRES
 * buf can store upto n + 1 chars
 *
 * MODIFIES
 * sb, buf
 *
 * EFFECTS
 * writes (at most) n chars to buf using sb.
 * returns the number of chars written.
 */
size_t
writeLWStringBuilder(LWStringBuilder *sb, char *buf, size_t n)
{
    if (n >= LWStringBuilderGetSize(sb)) {
        /* write all if buf can store the entire result */
        writeAll(sb, buf);
        return n;
    }
    return writeSome(sb, buf, n);
}

/*
 * MODIFIES
 * sb, outLen
 *
 * EFFECTS
 *
 */
char *
LWStringBuilderToString(LWStringBuilder *sb, size_t *outLen)
{
    char *ret;
    size_t retSize;
    size_t retAllocSize;

    retAllocSize = LWStringBuilderGetSize(sb) + 1;
    if (!(ret = malloc(retAllocSize)))
        goto error1;
    retSize = writeLWStringBuilder(sb, ret, retAllocSize - 1);
    if (outLen)
        *outLen = retSize;
    return ret;
error1:;
    return NULL;
}

static size_t
getSize(const LWStringBuilder *sb)
{
    size_t ret;
    size_t sizeofKey;
    size_t occurrences;

    ret = 0;
    for (size_t i = 0; i < getCountHashTable(sb->strs); i++) {
        sizeofKey = getSizeofKeyHashTable(sb->strs, i);
        occurrences = *(size_t *)getValueByKVIndexHashTable(sb->strs, i);
        /* exclude NULL terminators from count */
        ret += (sizeofKey + (sizeofKey > 1 ? -1 : 0)) * occurrences;
    }
    return ret;
}

size_t
LWStringBuilderGetSize(LWStringBuilder *sb)
{
    if (!sb->isDirty)
        return sb->size;
    sb->size = getSize(sb);
    sb->isDirty = 0;
    return sb->size;
}

int
LWStringBuilderPushStr(LWStringBuilder *sb, const char *str)
{
    size_t len;

    if (!*str)
        return 0;
    len = strlen(str);
    return len == 1 ? LWStringBuilderPushChar(sb, *str)
        : LWStringBuilderPushNStr(sb, str, len);
}

int
LWStringBuilderPushChar(LWStringBuilder *sb, const char c)
{
    if (!c)
        return 0;
    return LWStringBuilderPushNStr(sb, &c, 1);
}

/*
 * MODIFIES
 * sb
 *
 * EFFECTS
 * push a string (nStr characters long) into sb
 * returns non-zero on error
 */
int
LWStringBuilderPushNStr(LWStringBuilder *sb, const char *str, size_t nStr)
{
    size_t repCount;
    int kvIndex;
    LWSBRecord newRecord;
    size_t keySize;

    if (!*str)
        return 0;
    /* 
     * strings are mapped to the number of times they are pushed into the sb.
     * the records array keeps track of the order in which the strings were
     * pushed.
     */
    /* single character strings are interpreted as single chars */
    keySize = nStr > 1 ? nStr + sizeof(char) : nStr;
    /* check if str is alread in sb */
    kvIndex = getHashTable(sb->strs, (uint8_t *)str, keySize);
    if (kvIndex < 0) {
        /* str is not in sb, so insert it. */
        repCount = 1;
        if ((kvIndex = insertHashTable(sb->strs, (uint8_t *)str, keySize,
            (uint8_t *)&repCount, sizeof(size_t))) < 0) {
            return -1;
        }
    } else {
        /* str is in sb. Increment the occurrence counter. */
        (*(size_t *)getValueByKVIndexHashTable(sb->strs, kvIndex))++;
    }
    /* setup record */
    newRecord = (LWSBRecord) {
        .kvIndex = kvIndex,
    };
    sb->isDirty = 1;
    /* push new record  */
    return !(tryPushArray(&sb->records, &newRecord));
}

void
deleteLWStringBuilder(LWStringBuilder *sb)
{
    if (!sb)
        return;
    deleteArray(sb->records);
    deleteHashTable(sb->strs);
    free(sb);
}

/*
 *
 * MODIFIES
 * stdout
 *
 * EFFECTS
 * constructs sb and writes it to stdout
 */
/* XXX IDEA, save a cahced version of the pre-constructed string, 
 * use that for printing */
void
LWStringBuilderPrintInplace(const LWStringBuilder *sb)
{
    size_t kvIndex;
    size_t nextStrSize;
    const char *nextStr;

    for (size_t i = 0; i < getCountArray(sb->records); i++) {
        /* get kv details */
        kvIndex = ((LWSBRecord *)getElementArray(sb->records, i))->kvIndex;
        nextStr = (const char *)getKeyHashTable(sb->strs, kvIndex);
        nextStrSize = getSizeofKeyHashTable(sb->strs, kvIndex);
        /* check if the string is a string or just one char */
        if (nextStrSize == 1) {
            /* 
             * the next str is actually just one char.
             * if it is NULL, ignore it
             */
            if (!*nextStr)
                continue;
            /* print the char */
            putchar(*nextStr);
        } else {
            /* the next str is a null terminated str; print it */
            fwrite(nextStr, sizeof(char), nextStrSize - 1, stdout);
        }
    }
}

// #endif
