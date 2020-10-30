
#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/signal.h>
#include <ng/irq.h>
#include <ng/x86/cpu.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/pic.h>
#include <ng/x86/pit.h>
#include <ng/x86/uart.h>
#include <stdio.h>
#include <string.h>

#define USING_PIC 1

#ifdef USING_PIC
#define send_eoi pic_send_eoi
#else
#define send_eoi(...) (*(volatile u32 *)0xfee000b0 = 0)
#endif

// Stack dumps are not particularly helpful in the general case. This could be
// a runtime option though, it's a good candidate for that system when it
// happens.
#define DO_STACK_DUMP 0

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr_syscall(void);
extern void isr_yield(void);
extern void isr_panic(void);

extern void break_point(void);

void raw_set_idt_gate(uint64_t *idt, int index, void (*handler)(void),
                      uint64_t flags, uint64_t cs, uint64_t ist) {
        uint64_t *at = idt + index * 2;

        uint64_t h = (uint64_t)handler;
        uint64_t handler_low = h & 0xFFFF;
        uint64_t handler_med = (h >> 16) & 0xFFFF;
        uint64_t handler_high = h >> 32;

        at[0] = handler_low | (cs << 16) | (ist << 32) | (flags << 40) |
                (handler_med << 48);
        at[1] = handler_high;
}

enum idt_gate_flags {
        NONE = 0,
        USER_MODE = (1 << 0),
        STOP_IRQS = (1 << 1),
};

void register_idt_gate(int index, void (*handler)(void), int opts) {
        // TODO put these in a header
        uint16_t selector = 8; // kernel CS
        uint8_t rpl = (opts & USER_MODE) ? 3 : 0;
        uint8_t type = (opts & STOP_IRQS) ? 0xe : 0xf; // interrupt vs trap gates

        uint64_t flags = 0x80 | rpl << 5 | type;

        extern uint64_t idt[];
        raw_set_idt_gate(idt, index, handler, flags, selector, 0);
}

void install_isrs() {
        register_idt_gate(0, isr0, STOP_IRQS);
        register_idt_gate(1, isr1, STOP_IRQS);
        register_idt_gate(2, isr2, STOP_IRQS);
        register_idt_gate(3, isr3, STOP_IRQS);
        register_idt_gate(4, isr4, STOP_IRQS);
        register_idt_gate(5, isr5, STOP_IRQS);
        register_idt_gate(6, isr6, STOP_IRQS);
        register_idt_gate(7, isr7, STOP_IRQS);
        register_idt_gate(8, isr8, STOP_IRQS);
        register_idt_gate(9, isr9, STOP_IRQS);
        register_idt_gate(10, isr10, STOP_IRQS);
        register_idt_gate(11, isr11, STOP_IRQS);
        register_idt_gate(12, isr12, STOP_IRQS);
        register_idt_gate(13, isr13, STOP_IRQS);
        register_idt_gate(14, isr14, STOP_IRQS);
        register_idt_gate(15, isr15, STOP_IRQS);
        register_idt_gate(16, isr16, STOP_IRQS);
        register_idt_gate(17, isr17, STOP_IRQS);
        register_idt_gate(18, isr18, STOP_IRQS);
        register_idt_gate(19, isr19, STOP_IRQS);
        register_idt_gate(20, isr20, STOP_IRQS);
        register_idt_gate(21, isr21, STOP_IRQS);
        register_idt_gate(22, isr22, STOP_IRQS);
        register_idt_gate(23, isr23, STOP_IRQS);
        register_idt_gate(24, isr24, STOP_IRQS);
        register_idt_gate(25, isr25, STOP_IRQS);
        register_idt_gate(26, isr26, STOP_IRQS);
        register_idt_gate(27, isr27, STOP_IRQS);
        register_idt_gate(28, isr28, STOP_IRQS);
        register_idt_gate(29, isr29, STOP_IRQS);
        register_idt_gate(30, isr30, STOP_IRQS);
        register_idt_gate(31, isr31, STOP_IRQS);

        register_idt_gate(32, irq0, STOP_IRQS);
        register_idt_gate(33, irq1, STOP_IRQS);
        register_idt_gate(34, irq2, STOP_IRQS);
        register_idt_gate(35, irq3, STOP_IRQS);
        register_idt_gate(36, irq4, STOP_IRQS);
        register_idt_gate(37, irq5, STOP_IRQS);
        register_idt_gate(38, irq6, STOP_IRQS);
        register_idt_gate(39, irq7, STOP_IRQS);
        register_idt_gate(40, irq8, STOP_IRQS);
        register_idt_gate(41, irq9, STOP_IRQS);
        register_idt_gate(42, irq10, STOP_IRQS);
        register_idt_gate(43, irq11, STOP_IRQS);
        register_idt_gate(44, irq12, STOP_IRQS);
        register_idt_gate(45, irq13, STOP_IRQS);
        register_idt_gate(46, irq14, STOP_IRQS);
        register_idt_gate(47, irq15, STOP_IRQS);

        register_idt_gate(128, isr_syscall, USER_MODE);
        register_idt_gate(130, isr_panic, STOP_IRQS);
}

bool doing_exception_print = false;

void panic_trap_handler(interrupt_frame *r);

void c_interrupt_shim(interrupt_frame *r) {
        // printf("Interrupt %i\n", r->interrupt_number);
        bool from_usermode = false;
        
        if (r->ds > 0) {
                from_usermode = true;
                running_thread->user_sp = r->user_sp;
                running_thread->user_ctx = r;
                running_thread->flags |= TF_USER_CTX_VALID;
        }

        if (r->interrupt_number == 14) {
                page_fault(r);
        }
        else if (r->interrupt_number == 128) {
                syscall_handler(r);
        }
        else if (r->interrupt_number == 130) {
                panic_trap_handler(r);
        }
        else if (r->interrupt_number < 32) {
                generic_exception(r);
        }
        else if (r->interrupt_number == 32) {
                // timer interrupt needs to EOI first
                send_eoi(r->interrupt_number - 32);
                irq_handler(r);
        }
        else if (r->interrupt_number < 32 + NIRQS) {
                irq_handler(r);
                send_eoi(r->interrupt_number - 32);
        }

        if (from_usermode) {
                running_thread->flags &= ~TF_USER_CTX_VALID;
        }
}

void syscall_handler(interrupt_frame *r) {
        sysret ret;
        int syscall_num = frame_get(r, ARG0);
        syscall_entry(r, syscall_num);

        ret = do_syscall_with_table(
                frame_get(r, ARG0),
                frame_get(r, ARG1),
                frame_get(r, ARG2),
                frame_get(r, ARG3),
                frame_get(r, ARG4),
                frame_get(r, ARG5),
                frame_get(r, ARG6),
                r
        );

        frame_set(r, RET_VAL, ret);
        syscall_exit(r, syscall_num);
        handle_pending_signals();
}

void panic_trap_handler(interrupt_frame *r) {
        disable_irqs();
        printf("\n");
        printf("panic: trap at %#lx\n", r->ip);
        print_registers(r);
        printf("backtrace from: %#lx\n", r->bp);
        backtrace_from_with_ip(r->bp, 20, r->ip);
        panic();
}

const char *exception_codes[] = {
        "#DE",
        "#DB",
        "NMI",
        "#BP",
        "#OF",
        "#BR",
        "#UD",
        "#NM",
        "#DF",
        "<none>",
        "#TS",
        "#NP",
        "#SS",
        "#GP",
        "#PF",
        "<reserved>",
        "#MF",
        "#AC",
        "#MC",
        "#XM",
        "#VE",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "<reserved>",
        "#SX",
        "<reserved>"
};

const char *exception_reasons[] = {
        "Divide by zero",
        "Debug",
        "Non-maskable Interrupt",
        "Breakpoint",
        "Overflow Trap",
        "Bound Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun (Deprecated)",
        "Invalid TSS",
        "Segment Not Present",
        "Stack-Segment Fault",
        "General Protection Fault",
        "Page Fault",
        "Reserved",
        "x87 Floating Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception",
        "Virtualization Exception",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Security Exception",
        "Reserved",
};

static void print_error_dump(interrupt_frame *r) {
        uintptr_t ip = r->ip;
        uintptr_t bp = r->bp;
        printf("Fault occured at %#lx\n", ip);
        print_registers(r);
        printf("backtrace from: %#lx\n", bp);
        backtrace_from_with_ip(bp, 20, ip);
        uintptr_t real_sp = r->user_sp;

#if DO_STACK_DUMP
        printf("Stack dump: (sp at %#lx)\n", real_sp);
        dump_mem((char *)real_sp - 64, 128);
#endif
}

static noreturn void kill_for_unhandled_interrupt(interrupt_frame *r) {
        if ((r->cs & 3) > 0) {
                // died in usermode
                signal_self(SIGSEGV);
        } else if (running_process->pid > 0) {
                // died in kernel mode in a user process
                printf("Would signal SEGV, but we decided that was a bad idea\n");
                kill_process(running_process, 256 + SIGSEGV);
        } else {
                // died in kernel mode
                panic();
        }
        UNREACHABLE();
}

void page_fault(interrupt_frame *r) {
        uintptr_t fault_addr;
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

        const int PRESENT = 0x01;
        const int WRITE = 0x02;
        const int USERMODE = 0x04;
        const int RESERVED = 0x08;
        const int IFETCH = 0x10;

        // The vmm may choose to create pages that are not backed.  They will
        // appear as not having the PRESENT bit set.  In this case, ask it
        // if it can handle the situation and exit succesfully if it can
        extern int vmm_do_page_fault(uintptr_t fault_addr);
        if (vmm_do_page_fault(fault_addr)) {
                // handled and able to return
                return;
        }
        int code = r->error_code;

        if (r->error_code & RESERVED) {
                printf("Fault was caused by writing to a reserved field\n");
        }
        char *reason = code & PRESENT ? "protection violation" : "page not present";
        char *rw = code & WRITE ? "writing" : "reading";
        char *mode = code & USERMODE ? "user" : "kernel";
        char *type = code & IFETCH ? "instruction" : "data";

        printf("Thread: [%i:%i] (\"%s\") performed an access violation\n",
                running_process->pid, running_thread->tid, running_process->comm);

        if (code & USERMODE) {
                print_error_dump(r);
                signal_self(SIGSEGV);
        }

        const char *sentence = "Fault %s %s:%#lx because %s from %s mode.\n";
        printf(sentence, rw, type, fault_addr, reason, mode);

        if (fault_addr < 0x1000) {
                printf("NULL pointer access?\n");
        }
        break_point();
        print_error_dump(r);
        kill_for_unhandled_interrupt(r);
}

void generic_exception(interrupt_frame *r) {
        printf("Thread: [%i:%i] (\"%s\") experienced a fault\n",
                running_process->pid, running_thread->tid, running_process->comm);
        printf("Unhandled exception at %#lx\n", r->ip);
        printf("Fault: %s (%s), error code: %#04x\n",
               exception_codes[r->interrupt_number],
               exception_reasons[r->interrupt_number], r->error_code);

        break_point();
        print_error_dump(r);
        kill_for_unhandled_interrupt(r);
}

void unhandled_interrupt_handler(interrupt_frame *r) {

}

/* Utility functions */

void enable_irqs(void) {
        // printf("[e%i]", running_thread->irq_disable_depth);
        running_thread->irq_disable_depth -= 1;
        assert(running_thread->irq_disable_depth >= 0);
        if (running_thread->irq_disable_depth == 0) {
                asm volatile ("sti");
        }
}

void disable_irqs(void) {
        // printf("[d%i]", running_thread->irq_disable_depth);
        asm volatile ("cli");
        running_thread->irq_disable_depth += 1;
}

__USED uintptr_t dr6() {
        uintptr_t result;
        asm volatile (
                "mov %%dr6, %0 \n\t"
                : "=r"(result)
        );

        return result;
}
