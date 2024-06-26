#pragma once

#include "cpu.h"
#include "sys/cdefs.h"

BEGIN_DECLS

extern const char *exception_codes[];
extern const char *exception_reasons[];

void idt_install();
void enable_irqs();
void disable_irqs();
bool irqs_are_disabled();
void c_interrupt_shim(interrupt_frame *r);

// Moves va args 1, 2, 3 to userland args 1, 2, 3
void jmp_to_userspace(uintptr_t ip, uintptr_t sp, ...);
void page_fault(interrupt_frame *r);
void generic_exception(interrupt_frame *r);
void syscall_handler(interrupt_frame *r);

END_DECLS
