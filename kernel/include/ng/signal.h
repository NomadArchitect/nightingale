#pragma once

#include <ng/cpu.h>
#include <signal.h>
#include <sys/cdefs.h>
#include <sys/syscall_consts.h>

BEGIN_DECLS

struct thread;

struct signal_context {
	int state;
	void *sp;
	void *bp;
	char *stack;
	uintptr_t ip;
};

int signal_send(pid_t pid, int signal);
int signal_send_pgid(pid_t pgid, int signal);
int signal_send_th(struct thread *th, int signal);
void signal_self(int signal);

int handle_pending_signals();
void handle_signal(int signal, sighandler_t);

void do_signal_call(int signal, sighandler_t);

END_DECLS
