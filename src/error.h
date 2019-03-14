// error.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// error message handling

#ifndef LSIM_ERROR_H
#define LSIM_ERROR_H

#define ERROR_MSG(fmt, ...) error_msg(__FILE__, (fmt), __VA_ARGS__)

void error_msg(const char *file, const char *fmt, ...);

#endif // LSIM_ERROR_H
