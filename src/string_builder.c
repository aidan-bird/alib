#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "./string_builder.h"
#include "./array.h"
#include "./vlarray.h"

enum SBTypes
{
    SBTypes_str,
    SBTypes_char,
};

typedef enum SBTypes SBTypes;
typedef struct SBRecord SBRecord;

struct SBRecord
{
    SBTypes t;
};

struct StringBuilder
{
    Array *chars;
    VLArray *strs;
    Array *records;
};

size_t
stringBuilderGetSize(StringBuilder *sb)
{
    return sizeofArray(sb->chars) + getSizeVLArray(sb->strs)
        - getCountVLArray(sb->strs);
}

int
stringBuilderPushStr(StringBuilder *sb, const char *str)
{
    SBRecord nextRec;

    nextRec = (SBRecord) {
        .t = SBTypes_str
    };
    return !tryPushArray(&sb->records, &nextRec)
        || !tryPushVLArray(&sb->strs, str, strlen(str) + 1);
}

int
stringBuilderPushChar(StringBuilder *sb, const char c)
{
    SBRecord nextRec;

    nextRec = (SBRecord) {
        .t = SBTypes_char
    };
    return !tryPushArray(&sb->records, &nextRec)
        || !tryPushArray(&sb->chars, &c);
}

void
stringBuilderPrintInplace(const StringBuilder *sb)
{
    const SBRecord *nextRec;
    const char *nextStr;
    size_t istr;
    size_t ichar;

    istr = 0;
    ichar = 0;
    for (size_t i = 0; i < getCountArray(sb->records); i++) {
        nextRec = (const SBRecord *)getElementArray(sb->records, i);
        switch (nextRec->t) {
            case SBTypes_str:
                nextStr = (const char *)getElementVLArray(sb->strs, istr);
                printf("%s", nextStr);
                istr++;
                break;
            case SBTypes_char:
                putchar(*((char *)(getElementArray(sb->chars, ichar))));
                ichar++;
                break;
            default:
                break;
        }
    }
}

char *
stringBuilderToString(StringBuilder *sb, size_t *outLen)
{
    const SBRecord *nextRec;
    char *ret;
    const char *nextStr;
    size_t writeIndex;
    size_t nextStrSize;
    size_t istr;
    size_t ichar;

    writeIndex = 0;
    istr = 0;
    ichar = 0;
    ret = malloc(stringBuilderGetSize(sb) + 1);
    if (!ret)
        goto error1;
    for (size_t i = 0; i < getCountArray(sb->records); i++) {
        nextRec = (const SBRecord *)getElementArray(sb->records, i);
        switch (nextRec->t) {
            case SBTypes_str:
                nextStrSize = sizeOfElementVLArray(sb->strs, istr) - 1;
                nextStr = (const char *)getElementVLArray(sb->strs, istr);
                memcpy(ret + writeIndex, nextStr, nextStrSize);
                writeIndex += nextStrSize;
                istr++;
                break;
            case SBTypes_char:
                ret[writeIndex] = *(char *)(getElementArray(sb->chars, ichar));
                writeIndex++;
                ichar++;
                break;
            default:
                goto error2;
        }
    }
    ret[writeIndex] = '\0';
    if (outLen)
        *outLen = writeIndex;
    return ret;
error2:;
    free(ret);
error1:;
    return NULL;
}

StringBuilder *
newStringBuilder()
{
    StringBuilder *ret;

    ret = malloc(sizeof(StringBuilder));
    if (!ret)
        goto error1;
    ret->chars = newArray(-1, -1, sizeof(char));
    if (!ret->chars)
        goto error2;
    ret->records = newArray(-1, -1, sizeof(SBRecord));
    if (!ret->records)
        goto error3;
    ret->strs = newVLArray(-1, -1, -1);
    if (!ret->strs)
        goto error4;
    return ret;
error4:;
    deleteArray(ret->records);
error3:;
    deleteArray(ret->chars);
error2:;
    free(ret);
error1:;
    return NULL;
}

void
deleteStringBuilder(StringBuilder *sb)
{
    deleteArray(sb->records);
    deleteArray(sb->chars);
    deleteVLArray(sb->strs);
    free(sb);
}

