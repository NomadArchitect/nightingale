
#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <basic.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/dmgr.h>
#include <ng/list.h>
#include <ng/ringbuf.h>
#include <ng/vector.h>
#include <ng/tty.h>
// #include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

enum filetype {
        FT_CHARDEV,
        FT_TTY,
        FT_BUFFER,
        FT_SOCKET,
        FT_DIRECTORY,
        FT_PIPE,
        FT_PROC,
};

#define MAX_FILENAME 64

#define ALL_READ   0x0004
#define ALL_WRITE  0x0002
#define ALL_EXEC   0x0001
#define GRP_READ   0x0040
#define GRP_WRITE  0x0020
#define GRP_EXEC   0x0010
#define USR_READ   0x0400
#define USR_WRITE  0x0200
#define USR_EXEC   0x0100

#define SUID       0x1000
#define SGID       0x2000

typedef int64_t off_t;

struct file;
struct open_file;

enum file_flags {
        FILE_NONBLOCKING = 0x01,
};

struct file {
        int filetype;
        char filename[MAX_FILENAME];
        atomic_int refcnt;

        int signal_eof;

        int flags;
        int permission;
        int uid;
        int gid;

        off_t len;

        int (*open)(struct open_file *n);
        int (*close)(struct open_file *n);
        ssize_t (*read)(struct open_file *n, void *data, size_t len);
        ssize_t (*write)(struct open_file *n, const void *data, size_t len);
        off_t (*seek)(struct open_file *n, off_t offset, int whence);

        struct list blocked_threads;
        struct file *parent;

        struct ringbuf ring;    // FT_TTY
        struct tty *tty;        // FT_TTY
        void *memory;           // FT_BUFFER
        off_t capacity;         // FT_BUFFER
        struct list children;   // FT_DIRECTORY
};

struct open_file {
        struct file *node;
        void *buffer; // only used in procfs for now
        int flags;
        off_t off;
};

extern struct file *dev_serial;
extern struct open_file *ofd_stdin;
extern struct open_file *ofd_stdout;
extern struct open_file *ofd_stderr;

// seek

enum {
        SEEK_SET,
        SEEK_CUR,
        SEEK_END,
};

// poll

struct pollfd {
        int fd;
        short events;
        short revents;
};

enum {
        POLLIN,
};

typedef int nfds_t;

extern struct dmgr file_table;
extern struct file *fs_root_node;

void vfs_init();
void mount(struct file *n, char *path);

struct file *fs_resolve_relative_path(struct file *root, const char *filename);
void vfs_print_tree(struct file *root, int indent);

#endif // NG_FS_H

