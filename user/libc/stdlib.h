
#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <ng/basic.h>

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (1)

void *malloc(size_t len);
void free(void *alloc);
void *realloc(void *alloc, size_t len);
void *calloc(size_t count, size_t len);

int abs(int x);
long labs(long x);
long long llabs(long long x);

struct div_t {
        int quot;
        int rem;
};
struct ldiv_t {
        long quot;
        long rem;
};
struct lldiv_t {
        long long quot;
        long long rem;
};

typedef struct div_t div_t;
typedef struct ldiv_t ldiv_t;
typedef struct lldiv_t lldiv_t;

div_t div(int x, int y);
ldiv_t ldiv(long x, long y);
lldiv_t lldiv(long long x, long long y);

char *getenv(const char *name);

#endif
