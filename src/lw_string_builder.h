#ifndef LW_STRING_BUILDER_H
#define LW_STRING_BUILDER_H

#include <stddef.h>

typedef struct LWStringBuilder LWStringBuilder;

LWStringBuilder *newLWStringBuilder();
size_t writeLWStringBuilder(LWStringBuilder *sb, char *buf, size_t n);
char *LWStringBuilderToString(LWStringBuilder *sb, size_t *outLen);
size_t LWStringBuilderGetSize(LWStringBuilder *sb);
int LWStringBuilderPushStr(LWStringBuilder *sb, const char *str);
int LWStringBuilderPushChar(LWStringBuilder *sb, const char c);
int LWStringBuilderPushNStr(LWStringBuilder *sb, const char *str, size_t nStr);
void deleteLWStringBuilder(LWStringBuilder *sb);
void LWStringBuilderPrintInplace(const LWStringBuilder *sb);

#endif
