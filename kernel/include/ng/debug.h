#pragma once
#ifndef NG_DEBUG_H
#define NG_DEBUG_H

#include <basic.h>
#include <stdio.h>

#ifdef DEBUG

#define do_debug true
#define DEBUG_PRINTF(...)                                                      \
    do { printf("[DEBUG] " __VA_ARGS__); } while (0)

#else // !DEBUG

#define do_debug false
#define DEBUG_PRINTF(...)

#endif // DEBUG

#define WARN_PRINTF(...)                                                       \
    do { printf("[WARN!] " __VA_ARGS__); } while (0)

#define UNREACHABLE() assert("not reachable" && 0)

void backtrace_from_here(int max_frames);
void backtrace_from_with_ip(uintptr_t bp, int max_frames, uintptr_t ip);
void print_perf_trace(uintptr_t bp, uintptr_t ip);

int dump_mem(void *ptr, size_t len);
int hexdump(size_t len, char ptr[len]);

__NOINLINE void break_point();

#endif // NG_DEBUG_H
