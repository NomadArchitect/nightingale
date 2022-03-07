#pragma once
#include "types.h"

#include <fcntl.h>
#include <sys/types.h>

struct file_operations {
    ssize_t (*read)(struct fs2_file *, char *buffer, size_t len);
    ssize_t (*write)(struct fs2_file *, const char *buffer, size_t len);
};

extern struct file_operations default_file_ops;

struct fs2_file {
    struct dentry *dentry;
    struct inode *inode;
    const struct file_operations *ops;
    enum open_flags flags;

    off_t offset;
    void *extra;
};

// Get a fs2_file from the running_process's fd table
struct fs2_file *get_file(int fd);
int add_file(struct fs2_file *fd);
struct fs2_file *remove_file(int fd);

ssize_t default_read(struct fs2_file *, char *, size_t);
ssize_t default_write(struct fs2_file *, const char *, size_t);


bool read_permission(struct fs2_file *fs2_file);
bool write_permission(struct fs2_file *fs2_file);
bool execute_permission(struct fs2_file *fs2_file);