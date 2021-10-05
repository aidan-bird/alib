/* aidan bird 2021 */
#include <stdlib.h>
#include <string.h>

#include "./csv.h"
#include "./utils.h"

/*
 * 
 */
VLArray *
newCSVRaw(const void *csvtext, size_t n)
{
    VLArray *ret;
    VLArray *tmp;
    size_t *entryIndexes;
    size_t *entrySizes;
    size_t entryCount;
    size_t entryOffsetEnd;
    size_t entryOffsetStart;
    size_t maxEntrySize;
    char *nextEntry;

    entryCount = charCount(csvtext, n, ',') + 1;
    if (!(ret = newVLArray(-1, entryCount, (n / entryCount) + 1)))
        goto error1;
    if (!(entryIndexes = malloc(2 * sizeof(size_t) * entryCount)))
        goto error2;
    entrySizes = (size_t *)((char *)entryIndexes 
        + sizeof(size_t) * entryCount);
    entryOffsetStart = 0;
    entryOffsetEnd = 0;
    maxEntrySize = 0;
    for (size_t i = 0; i < entryCount; i++) {
        entryIndexes[i] = entryOffsetStart;
        while (entryOffsetEnd < n
            && ((const char *)csvtext)[entryOffsetEnd] != ',') {
            entryOffsetEnd++;
        }
        entrySizes[i] = entryOffsetEnd - entryOffsetStart;
        if (maxEntrySize < entrySizes[i])
            maxEntrySize = entrySizes[i];
        entryOffsetEnd++;
        entryOffsetStart = entryOffsetEnd;
    }
    if (!(nextEntry = malloc(sizeof(maxEntrySize + 1))))
        goto error3;
    for (size_t i = 0; i < entryCount; i++) {
        memcpy(nextEntry, (char *)csvtext + entryIndexes[i], entrySizes[i]);
        nextEntry[entrySizes[i]] = '\0';
        tmp = pushVLArray(ret, nextEntry, entrySizes[i] + 1);
        if (!tmp)
            goto error4;
        ret = tmp;
    }
    free(nextEntry);
    free(entryIndexes);
    return ret;
error4:;
    free(nextEntry);
error3:;
    free(entryIndexes);
error2:;
    deleteVLArray(ret);
error1:;
    return NULL;
}
