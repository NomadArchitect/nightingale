#pragma once

#include <ng/cpu.h>
#include <ng/string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/syscall_consts.h>

BEGIN_DECLS

#define SYSCALL_TABLE_SIZE 256

typedef intptr_t sysret;

void syscall_entry(int);
void syscall_exit(int);

int syscall_register(int num, const char *name, sysret (*)(), const char *debug,
	unsigned ptr_mask);

sysret do_syscall(interrupt_frame *);

END_DECLS
