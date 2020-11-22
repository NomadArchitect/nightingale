#pragma once
#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

struct ng_dirent {
    enum filetype type;
    enum file_permission permissions;
    char filename[64];
};

#ifndef __kernel__

ssize_t getdirents(int fd, struct ng_dirent *buf, size_t count);

ssize_t readdir(int fd, struct ng_dirent *buf, size_t count);

#endif

#endif // _DIRENT_H_
