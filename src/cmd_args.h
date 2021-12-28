#ifdef DEBUG

#ifndef ALIB_CMD_ARGS_H
#define ALIB_CMD_ARGS_H

#include "./array.h"

typedef struct CmdArgSpec CmdArgSpec;
typedef struct CmdArgParseResult CmdArgParseResult;
typedef void (*CmdArgErrorFunc)();
typedef int (*CmdArgParserFunc)();

struct CmdArgSpec
{
    int onlyOnce;
    int terminatesParsing;
    const char *longForm;
    char shortForm;
    CmdArgErrorFunc error;
    CmdArgParserFunc parse;
};

/*
 * args := arg | arg args
 * arg := argLabel argValue | argLabel args
 */

struct CmdArgParseResult
{
    int cmdArgSpecID;
    union CmdArgParseResultValue {

    } result;
};

Array *parseArgs(const char **args, size_t n);

#endif
#endif
