#ifndef ALIB_STRING_BUILDER_H
#define ALIB_STRING_BUILDER_H

#include <stddef.h>

typedef struct StringBuilder StringBuilder;

StringBuilder *newStringBuilder();
char *stringBuilderToString(StringBuilder *sb, size_t *outLen);
size_t stringBuilderGetSize(StringBuilder *sb);
int stringBuilderPushStr(StringBuilder *sb, const char *str);
int stringBuilderPushChar(StringBuilder *sb, const char c);
void deleteStringBuilder(StringBuilder *sb);
void stringBuilderPrintInplace(const StringBuilder *sb);

#endif
