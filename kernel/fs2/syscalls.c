#include <basic.h>
#include <dirent.h>
#include <fcntl.h>
#include <list.h>
#include <ng/string.h>
#include <ng/thread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "char_dev.h"
#include "dentry.h"
#include "inode.h"
#include "file.h"
#include "file_system.h"
#include "pipe.h"

// associate inode with NEGATIVE dentry
struct fs2_file *create_file2(
    struct dentry *dentry,
    struct inode *inode,
    int flags
);
// open existing inode
struct fs2_file *new_file(struct dentry *dentry, int flags);
// Create a file for an inode that has NO dentry (i.e. pipe)
struct fs2_file *no_d_file(struct inode *inode, int flags);
// truncate fs2_file
void truncate(struct fs2_file *fs2_file);
// set to append, move cursor
void append(struct fs2_file *fs2_file);

sysret do_open2(struct dentry *cwd, const char *path, int flags, int mode) {
    struct dentry *dentry = resolve_path_from(cwd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);

    if (!inode && !(flags & O_CREAT)) {
        return -ENOENT;
    }
    if (dentry && inode && flags & O_CREAT && flags & O_EXCL) {
        return -EEXIST;
    }

    struct file_system *file_system = dentry_file_system(dentry);
    struct fs2_file *fs2_file;

    if (inode && !has_permission(inode, flags))
        return -EPERM;
    else if (
        !inode &&
        !has_permission(dentry_inode(dentry->parent), flags | O_WRONLY)
    )
        return -EPERM;

    if (!inode && flags & O_CREAT) {
        inode = new_inode(file_system, mode);
        fs2_file = create_file2(dentry, inode, flags);
    } else {
        fs2_file = new_file(dentry, flags);
    }

    if (flags & O_TRUNC)
        truncate(fs2_file);

    if (flags & O_APPEND)
        append(fs2_file);

    open_file(fs2_file);

    return add_file(fs2_file);
}

sysret sys_openat2(int fd, const char *path, int flags, int mode) {
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open2(root, path, flags, mode);
}

sysret sys_touchat2(int fd, const char *path, int mode) {
    // create a file like open(O_CREAT), potentially with non-NORMAL
    // mode, but don't open it or return an fd. This could potentially
    // be used to back mkdir(3) and mkdirat(3)
    return -ETODO;
}

sysret sys_mkdirat2(int fd, const char *path, int mode) {
    struct dentry *root = resolve_atfd(fd);

    if (IS_ERROR(root))
        return ERROR(root);

    return do_open2(root, path, O_CREAT | O_EXCL, _NG_DIR | mode);
}

sysret sys_close2(int fd) {
    struct fs2_file *file = remove_file(fd);
    if (!file)
        return -EBADF;
    close_file(file);
    return 0;
}



sysret sys_getdents2(int fd, struct ng_dirent *dents, size_t len) {
    struct fs2_file *directory = get_file(fd);
    if (!directory)
        return -EBADF;

    struct inode *inode = directory->inode;

    if (inode->type != FT_DIRECTORY)
        return -ENOTDIR;

    if (!execute_permission(inode))
        return -EPERM;

    size_t index = 0;
    list_for_each(
        struct dentry,
        d,
        &directory->dentry->children,
        children_node
    ) {
        if (!d->inode) {
            continue;
        }
        strncpy(dents[index].name, d->name, 128);
        dents[index].type = d->inode->type;
        index += 1;

        if (index == len) {
            break;
        }
    }
    return index;
}

sysret sys_pathname2(int fd, char *buffer, size_t len) {
    struct fs2_file *fs2_file = get_file(fd);
    if (!fs2_file)
        return -EBADF;

    struct dentry *dentry = fs2_file->dentry;

    return pathname(fs2_file, buffer, len);
}

sysret sys_read2(int fd, char *buffer, size_t len) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return read_file(file, buffer, len);
}

sysret sys_write2(int fd, char *buffer, size_t len) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return write_file(file, buffer, len);
}

sysret sys_ioctl2(int fd, int request, void *argp) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return ioctl_file(file, request, argp);
}

sysret sys_lseek2(int fd, off_t offset, int whence) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    return seek_file(file, offset, whence);
}


sysret sys_fstat2(int fd, struct stat *stat) {
    struct fs2_file *file = get_file(fd);
    if (!file)
        return -EBADF;

    struct inode *inode = file->inode;
    *stat = (struct stat) {
        .st_dev = (dev_t)(intptr_t)inode->file_system,
        .st_ino = inode->inode_number,
        .st_mode = inode->mode,
        .st_nlink = inode->dentry_refcnt,
        .st_uid = inode->uid,
        .st_gid = inode->gid,
        .st_rdev = (inode->device_major << 16) + inode->device_minor,
        .st_size = inode->len,
        .st_blksize = 1024, // FIXME
        .st_blocks = round_up(inode->len, 1024) / 1024, // FIXME
        .st_atime = inode->atime,
        .st_mtime = inode->mtime,
        .st_ctime = inode->ctime,
    };
    return 0;
}

sysret sys_linkat2(
    int oldfdat,
    const char *oldpath,
    int newfdat,
    const char *newpath
) {
    struct dentry *olddentry = resolve_atpath(oldfdat, oldpath, true);
    if (IS_ERROR(olddentry))
        return ERROR(olddentry);

    if (dentry_inode(olddentry)->type == FT_DIRECTORY)
        return -EISDIR;

    struct dentry *newdentry = resolve_atpath(newfdat, newpath, true);
    if (IS_ERROR(newdentry))
        return ERROR(newdentry);

    if (dentry_inode(newdentry))
        return -EEXIST;

    if (newdentry->file_system != olddentry->file_system)
        return -EXDEV;

    attach_inode(newdentry, olddentry->inode);
    return 0;
}

sysret sys_symlinkat2(const char *topath, int newfdat, const char *newpath) {
    struct dentry *dentry = resolve_atpath(newfdat, newpath, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    if (dentry_inode(dentry))
        return -EEXIST;

    struct inode *inode = new_inode(dentry->file_system, _NG_SYMLINK | 0777);
    inode->symlink_destination = strdup(topath);
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_readlinkat2(int atfd, const char *path, char *buffer, size_t len) {
    struct dentry *dentry = resolve_atpath(atfd, path, false);
    if (IS_ERROR(dentry))
        return ERROR(dentry);
    struct inode *inode = dentry_inode(dentry);
    if (!inode)
        return -ENOENT;
    if (inode->type != FT_SYMLINK)
        return -EINVAL;

    strncpy(buffer, inode->symlink_destination, len);
    return strlen(buffer);
}

sysret sys_mknodat2(int atfd, const char *path, mode_t mode, dev_t device) {
    struct dentry *dentry = resolve_atpath(atfd, path, true);
    if (IS_ERROR(dentry))
        return ERROR(dentry);

    struct file_system *file_system = dentry_file_system(dentry);
    struct inode *inode = new_inode(file_system, mode);
    int device_major = device >> 16;
    struct file_operations *drv_ops = char_drivers[device_major];
    if (!drv_ops)
        return -ENODEV;

    inode->device_major = device_major;
    inode->device_minor = device & 0xFFFF;
    inode->file_ops = drv_ops;
    attach_inode(dentry, inode);
    return 0;
}

sysret sys_pipe2(int pipefds[static 2]) {
    struct inode *pipe = new_pipe();
    struct fs2_file *read_end = no_d_file(pipe, O_RDONLY);
    struct fs2_file *write_end = no_d_file(pipe, O_WRONLY);
    pipefds[0] = add_file(read_end);
    pipefds[1] = add_file(write_end);
    return 0;
}

sysret sys_mountat2(
    int atfd,
    const char *target,
    int type,
    int s_atfd,
    const char *source
) {
    struct dentry *tdentry = resolve_atpath(atfd, target, true);
    if (IS_ERROR(tdentry))
        return ERROR(tdentry);

    struct file_system *file_system;
#define _FS_PROCFS 1

    switch (type) {
    case _FS_PROCFS:
        if (proc_file_system->mounted_on)
            return -EBUSY;
        tdentry->mounted_file_system = proc_file_system;
        proc_file_system->mounted_on = tdentry;
        break;
    default:
        return -ETODO;
    }

    return 0;
}

sysret sys_dup_2(int fd) {
    struct fs2_file *old = get_file(fd);
    if (!old)
        return -EBADF;
    struct fs2_file *new = clone_file(old);
    return add_file(new);
}

sysret sys_dup2_2(int fd, int newfd) {
    struct fs2_file *old = get_file(fd);
    if (!old)
        return -EBADF;
    struct fs2_file *new = clone_file(old);

    struct fs2_file *close = get_file(newfd);
    if (close)
        close_file(close);
    return add_file_at(new, newfd);
}





// Given a POSITIVE dentry (extant inode), create a `struct file` to
// open it and return the new "file" object.
struct fs2_file *new_file(struct dentry *dentry, int flags) {
    struct fs2_file *fs2_file = malloc(sizeof(struct fs2_file));
    struct inode *inode = dentry_inode(dentry);
    *fs2_file = (struct fs2_file) {
        .inode = inode,
        .dentry = dentry,
        .flags = flags, // validate?
        .ops = dentry->inode->file_ops,
    };

    open_file(fs2_file);
    return fs2_file;
}

// Given a NEGATIVE dentry and an inode, associate the inode with the
// dentry, then open the file and return it.
struct fs2_file *create_file2(
    struct dentry *dentry,
    struct inode *inode,
    int flags
) {
    attach_inode(dentry, inode);
    return new_file(dentry, flags);
}

// Create a file for an inode that has NO dentry (i.e. pipe)
struct fs2_file *no_d_file(struct inode *inode, int flags) {
    struct fs2_file *fs2_file = malloc(sizeof(struct fs2_file));
    *fs2_file = (struct fs2_file) {
        .inode = inode,
        .flags = flags, // validate?
        .ops = inode->file_ops,
    };

    open_file(fs2_file);
    return fs2_file;
}


// truncate fs2_file
void truncate(struct fs2_file *fs2_file) {
    fs2_file->inode->len = 0;
}

// set to append, move cursor
void append(struct fs2_file *fs2_file) {
    fs2_file->offset = fs2_file->inode->len;
}
