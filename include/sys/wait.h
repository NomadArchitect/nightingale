#pragma once

#include <sys/cdefs.h>

#define WEXITSTATUS(stat_val) 0
#define WIFEXITED(stat_val) 0
#define WIFSIGNALED(stat_val) 0
#define WTERMSIG(stat_val) 0

enum wait_options {
	WNOHANG = 1,
};

BEGIN_DECLS

pid_t waitpid(pid_t pid, int *status, enum wait_options options);
pid_t wait(int *status);

END_DECLS
