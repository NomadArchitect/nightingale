#include <assert.h>
#include <dirent.h>
#include <ng/common.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

ssize_t readdir(int fd, struct dirent *buf, size_t count);

static pid_t redirect_child = 0;
static void wait_on_redirect_child() {
	fflush(stdout);
	waitpid(redirect_child, nullptr, 0);
}
/*
 * Spawns a new process based on the argument list provided.
 * Sends this process's stdout to the new's stdin.
 */
void redirect_output_to(char *const argv[]) {
	int pipefds[2];
	pipe(pipefds);
	pid_t child;

	if (redirect_child)
		return; // error?

	if ((child = fork()) == 0) {
		dup2(pipefds[0], STDIN_FILENO);
		close(pipefds[0]);
		close(pipefds[1]);
		execvp(argv[0], argv);
		assert("exec failed" && 0);
	} else {
		dup2(pipefds[1], STDOUT_FILENO);
		close(pipefds[0]);
		close(pipefds[1]);
		setvbuf(stdout, nullptr, _IOFBF, 0);
		redirect_child = child;
		atexit(wait_on_redirect_child);
	}
}
