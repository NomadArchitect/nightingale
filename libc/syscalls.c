#define FCNTL_NO_OPEN
#define IOCTL_NO_IOCTL
#define UNISTD_NO_GETCWD
#include "syscall.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <nightingale.h>
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall_consts.h>
#include <sys/trace.h>
#include <sys/ttyctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct submission;

#include "autogenerated_syscall_names.c"

static inline int is_error(intptr_t ret) { return (ret < 0 && ret > -0x1000); }

#define RETURN_OR_SET_ERRNO(ret) \
	if (!is_error(ret)) { \
		return ret; \
	} else { \
		errno = -ret; \
		return -1; \
	}

#include "autogenerated_syscalls_user.c"
