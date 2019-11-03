
#pragma once
#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <basic.h>
#include <unistd.h>

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (1)

void *malloc(size_t len);
void free(void *alloc);
void *realloc(void *alloc, size_t len);
void *calloc(size_t count, size_t len);

struct mregion;
typedef struct mregion mregion;

void malloc_initialize(mregion *, size_t len);
extern mregion *__malloc_pool;

void *pool_malloc(mregion *, size_t len);
void  pool_free(mregion *, void *alloc);
void *pool_realloc(mregion *, void *alloc, size_t len);
void *pool_calloc(mregion *, size_t count, size_t len);

void *zmalloc(size_t len);

#ifndef _NG

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

void abort(void);

long int random(void);
void srandom(unsigned seed);

double strtod(const char *str, char **end);
float strtof(const char *str, char **end);
long double strtold(const char *str, char **end);

long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *ntr, char **endptr, int base);

int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);


// TODO

int system(const char *command);
int mkstemp(char *name);

#endif // !_NG

#endif // _STDLIB_H_
