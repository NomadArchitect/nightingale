#include <stdlib.h>
#include "inode.h"
#include "dentry.h"
#include "file.h"

struct inode_operations default_ops = { 0 };

static int open_file_refcounts(struct fs2_file *file)
{
    // TODO: should the dentry refcounts be managed by inode.c?
    if (file->dentry)
        atomic_fetch_add(&file->dentry->file_refcnt, 1);

    if (file->flags & O_RDONLY)
        atomic_fetch_add(&file->inode->read_refcnt, 1);
    if (file->flags & O_WRONLY)
        atomic_fetch_add(&file->inode->write_refcnt, 1);
    return 0;
}

int open_file_clone(struct fs2_file *file) { return open_file_refcounts(file); }

int open_file(struct fs2_file *file)
{
    open_file_refcounts(file);

    if (file->inode->ops->open)
        file->inode->ops->open(file->inode, file);
    return 0;
}

int close_file(struct fs2_file *file)
{
    // TODO: should the dentry refcounts be managed by inode.c?
    if (file->dentry)
        atomic_fetch_sub(&file->dentry->file_refcnt, 1);

    if (file->flags & O_RDONLY)
        atomic_fetch_sub(&file->inode->read_refcnt, 1);
    if (file->flags & O_WRONLY)
        atomic_fetch_sub(&file->inode->write_refcnt, 1);

    if (file->inode->ops->close)
        file->inode->ops->close(file->inode, file);
    return 0;
}
