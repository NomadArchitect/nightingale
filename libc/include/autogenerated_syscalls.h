#ifndef _AUTOGENERATED_libc_include_autogenerated_syscalls_h_
#define _AUTOGENERATED_libc_include_autogenerated_syscalls_h_

noreturn void _exit(int exit_code);
int open(char *path, int flags, int mode);
ssize_t read(int fd, void *data, size_t len);
ssize_t write(int fd, const void *data, size_t len);
pid_t fork();
int top(int show_threads);
pid_t getpid();
pid_t gettid();
int execve(char *program, char *const *argv, char *const *envp);
int strace(int trace);
int waitpid(pid_t pid, int *exit_code, enum wait_options options);
int dup2(int fd_dest, int fd_src);
int uname(struct utsname *uname);
int yield();
off_t seek(int fd, off_t offset, int whence);
int poll(struct pollfd *pollfd, nfds_t nfds, int timeout);
void* mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t len);
int setpgid(pid_t pid, pid_t pgid);
noreturn void exit_group(int exit_code);
int clone0(clone_fn *fn, void *arg, void *new_stack, int flags);
int loadmod(int fd);
noreturn int haltvm(int exit_code);
int openat(int fd, const char *name, int flags);
int execveat(int fd, char *program, char *const *argv, char *const *envp);
int ttyctl(int fd, int command, int arg);
int close(int fd);
int pipe(int *pipefds);
sighandler_t sigaction(int sig, sighandler_t handler, int flags);
noreturn int sigreturn(int code);
int kill(pid_t pid, int dig);
int sleepms(int ms);
ssize_t readdir(int fd, struct ng_dirent *buf, size_t count);
long xtime();
pid_t create(const char *executable);
int procstate(pid_t pid, enum procstate flags);
int fault(enum fault_type fault);
int trace(enum trace_command cmd, pid_t pid, void *addr, void *data);
int sigprocmask(int op, const sigset_t *new, sigset_t *old);
int syscall_test(char *buffer);

#endif
