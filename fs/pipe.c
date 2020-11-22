#include <assert.h>
#include <basic.h>
#include <ng/fs.h>
#include <ng/ringbuf.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <stdlib.h>
#include <string.h>

void pipe_close(struct open_file *n) {
    struct file *file = n->node;
    assert(file->filetype == FT_PIPE);
    struct pipe_file *pipe = (struct pipe_file *)file;

    if (n->flags == USR_WRITE) { pipe->nwrite -= 1; }
    if (n->flags == USR_READ) { pipe->nread -= 1; }

    if (pipe->nwrite == 0) { file->signal_eof = 1; }

    if (n->node->refcnt <= 0) {
        free_ring(&pipe->ring);
        free(file);
        n->node = 0;
    }
}

ssize_t pipe_read(struct open_file *n, void *data, size_t len) {
    struct file *file = n->node;
    assert(file->filetype == FT_PIPE);
    struct pipe_file *pipe = (struct pipe_file *)file;

    len = ring_read(&pipe->ring, data, len);
    if (len == 0) return -1;
    return len;
}

ssize_t pipe_write(struct open_file *n, const void *data, size_t len) {
    struct file *file = n->node;
    assert(file->filetype == FT_PIPE);
    struct pipe_file *pipe = (struct pipe_file *)file;

    if (!pipe->nread) { signal_self(SIGPIPE); }
    len = ring_write(&pipe->ring, data, len);
    wake_waitq_all(&file->blocked_threads);
    return len;
}

void pipe_clone(struct open_file *parent, struct open_file *child) {
    struct file *file = parent->node;
    struct pipe_file *pipe = (struct pipe_file *)file;

    if (parent->flags == USR_WRITE) { pipe->nwrite += 1; }
    if (parent->flags == USR_READ) { pipe->nread += 1; }
}

struct file_ops pipe_ops = {
    .read = pipe_read,
    .write = pipe_write,
    .close = pipe_close,
    .clone = pipe_clone,
};

sysret sys_pipe(int pipefd[static 2]) {
    // create a pipe
    // create 2 open_files, one for each end
    // put them in the output

    struct pipe_file *pipe_file = zmalloc(sizeof(struct pipe_file));
    struct open_file *readfd = zmalloc(sizeof(struct open_file));
    struct open_file *writefd = zmalloc(sizeof(struct open_file));

    pipe_file->file.filetype = FT_PIPE;
    pipe_file->file.refcnt = 2; // don't free until both ends are closed
    pipe_file->file.permissions = 0;
    pipe_file->file.uid = 0; // running_process->euid
    pipe_file->file.ops = &pipe_ops;

    list_init(&pipe_file->file.blocked_threads);

    emplace_ring(&pipe_file->ring, 4096);

    // pipe_file has no parent and does not exist in the normal
    // directory tree.

    readfd->node = &pipe_file->file;
    writefd->node = &pipe_file->file;
    readfd->flags = USR_READ;
    writefd->flags = USR_WRITE;

    pipefd[0] = dmgr_insert(&running_process->fds, readfd);
    pipefd[1] = dmgr_insert(&running_process->fds, writefd);

    return 0;
}
