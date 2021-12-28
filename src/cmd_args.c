#ifdef debug

#include "./cmd_args.h"

#define NULL_SHORT_FORM '\0'

static const CmdArgSpec argSpec[] = {
    /* -m */
    [ARG_M] = {
        .onlyOnce = 1,
        .terminatesParsing = 0,
        .longForm = NULL,
        .shortForm = NULL_SHORT_FORM,
        .error = NULL,
        .parse = NULL,
    },
    /* -f FILE */
    [ARG_FILE] = {
        .onlyOnce = 0,
        .terminatesParsing = 0,
        .longForm = "-file",
        .shortForm = NULL_SHORT_FORM,
        .error = argsErrorFile,
        .parse = argsParseFile,
    },
    /* (EXPR) or (EXPR) */
    [ARG_FILE] = {
        .onlyOnce = 0,
        .terminatesParsing = 0,
        .longForm = "-or",
        .shortForm = NULL_SHORT_FORM,
        .error = argsErrorFile,
        .parse = argsParseFile,
    },
    /* -- */
    [ARG_TERM] = {
        .onlyOnce = 0,
        .terminatesParsing = 1,
        .longForm = "-",
        .shortForm = NULL_SHORT_FORM,
        .error = argsErrorFile,
        .parse = argsParseFile,
    },
};

/*
 * DESIGN GOALS
 *
 * define a machine that parses arguments at compile time
 *
 * the machine can a config defined at compile time
 *
 * the config determines which commands can only appear once
 *
 * arguments can have short and long forms
 *
 * arguments can have subarguments
 *
 * arguments can emit errors
 *
 * the result will be an array of parsed arguments which will be interpreted by 
 * the caller
 *
 * -- terminates argument parsing
 *
 */

/*
 * args := arg | arg args
 *
 * arg := single | multi
 *
 * single := - keyword
 *
 * multi := single subargs
 *
 * subargs := subarg | subarg subargs
 *
 * subarg := ...
 *
 * keyword := keyword1 | keyword2 | ...
 *
 */

/*
 * split argument parsing into two phases
 *
 * 1) gathering subarguments
 *
 * 2) parsing
 *
 * */


#endif
