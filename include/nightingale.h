#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>
#include <syscall_types.h>

BEGIN_DECLS

struct interrupt_frame;

extern const char *syscall_names[];

[[noreturn]] int haltvm(int exit_code);
long xtime();
pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);
int fault(enum fault_type type);
void print_registers(struct interrupt_frame *);
int syscall_trace(pid_t pid, int state);
int top(int show_threads);
int load_module(int fd);
int sleepms(int milliseconds);

void redirect_output_to(char *const argv[]);

END_DECLS
