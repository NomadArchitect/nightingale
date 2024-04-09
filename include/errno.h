#pragma once

#include "autogenerated_errnos.h"
#include <stdio.h>
#include <sys/cdefs.h>

#define EAGAIN EWOULDBLOCK

BEGIN_DECLS

extern const char *errno_names[];

#ifndef __kernel__
extern int errno;
void perror(const char *const message);
char *strerror(int errno);
#endif // __kernel__

END_DECLS
