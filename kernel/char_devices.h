
#pragma once
#ifndef NG_K_CHAR_DEVICES_H
#define NG_K_CHAR_DEVICES_H

#include <ng/basic.h>
#include <ng/print.h>
#include <ng/syscall.h>
#include <ng/ringbuf.h>
#include <ng/vector.h>
#include <ng/fs.h>
#include <stddef.h>
#include <stdint.h>

ssize_t dev_zero_read(struct open_fd *n, void *data_, size_t len);
ssize_t dev_null_write(struct open_fd *n, const void *data, size_t len);
ssize_t dev_inc_read(struct open_fd *n, void *data_, size_t len);
ssize_t dev_serial_write(struct open_fd *n, const void *data_, size_t len);
ssize_t dev_serial_read(struct open_fd *n, void *data_, size_t len);

#endif // NG_K_CHAR_DEVICES_H

