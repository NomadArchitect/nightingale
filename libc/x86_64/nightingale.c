#include <basic.h>
#include <nightingale.h>
#include <stdio.h>

#if __kernel__
#include <ng/debug.h>
#include <ng/thread.h>
#endif

void print_registers(interrupt_frame *r) {
    printf("    rax: %16lx    r8 : %16lx\n", r->rax, r->r8);
    printf("    rbx: %16lx    r9 : %16lx\n", r->rbx, r->r9);
    printf("    rcx: %16lx    r10: %16lx\n", r->rcx, r->r10);
    printf("    rdx: %16lx    r11: %16lx\n", r->rdx, r->r11);
    printf("    rsp: %16lx    r12: %16lx\n", r->user_sp, r->r12);
    printf("    rbp: %16lx    r13: %16lx\n", r->bp, r->r13);
    printf("    rsi: %16lx    r14: %16lx\n", r->rsi, r->r14);
    printf("    rdi: %16lx    r15: %16lx\n", r->rdi, r->r15);
    printf("    rip: %16lx    rfl: [", r->ip);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c] (%lx)\n",
           r->flags & 0x00000001 ? 'C' : ' ', r->flags & 0x00000004 ? 'P' : ' ',
           r->flags & 0x00000010 ? 'A' : ' ', r->flags & 0x00000040 ? 'Z' : ' ',
           r->flags & 0x00000080 ? 'S' : ' ', r->flags & 0x00000100 ? 'T' : ' ',
           r->flags & 0x00000200 ? 'I' : ' ', r->flags & 0x00000400 ? 'D' : ' ',
           r->flags & 0x00000800 ? 'O' : ' ', r->flags & 0x00010000 ? 'R' : ' ',
           r->flags & 0x00020000 ? 'V' : ' ', r->flags & 0x00040000 ? 'a' : ' ',
           r->flags & 0x00080000 ? 'v' : ' ', r->flags & 0x00100000 ? 'v' : ' ',
           r->flags);
#if __kernel__
    uintptr_t cr3 = 0;
    asm volatile("mov %%cr3, %0" : "=a"(cr3));
    printf("    cr3: %16lx    pid: [%i:%i]\n", cr3, running_process->pid,
           running_thread->tid);
#endif
}

uintptr_t frame_get(interrupt_frame *r, int reg) {
    switch (reg) {
        case ARG0:
            return r->rax;
        case ARG1:
            return r->rdi;
        case ARG2:
            return r->rsi;
        case ARG3:
            return r->rdx;
        case ARG4:
            return r->rcx;
        case ARG5:
            return r->r8;
        case ARG6:
            return r->r9;
        case RET_VAL:
            return r->rax;
        case RET_ERR:
            return r->rcx;
        case ARGC:
            return r->rdi;
        case ARGV:
            return r->rsi;
        case ENVP:
            return r->rdx;
    }
    return 0xE01E01E01; // error
}

uintptr_t frame_set(interrupt_frame *r, int reg, uintptr_t value) {
    switch (reg) {
        case ARG0:
            r->rax = value;
            break;
        case ARG1:
            r->rdi = value;
            break;
        case ARG2:
            r->rsi = value;
            break;
        case ARG3:
            r->rdx = value;
            break;
        case ARG4:
            r->rcx = value;
            break;
        case ARG5:
            r->r8 = value;
            break;
        case ARG6:
            r->r9 = value;
            break;
        case RET_VAL:
            r->rax = value;
            break;
        case RET_ERR:
            r->rcx = value;
            break;
        case ARGC:
            r->rdi = value;
            break;
        case ARGV:
            r->rsi = value;
            break;
        case ENVP:
            r->rdx = value;
            break;
    }
    return value;
}
