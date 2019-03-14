// error.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// error message handling

#include "error.h"
#include <cstdio>
#include <cstdarg>

void error_msg(const char *file, const char *fmt, ...)  {
    // just dump errors to stderr for now
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "ERROR (%s) : ", file);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}