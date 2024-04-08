#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <tar.h>

BEGIN_DECLS

uint64_t tar_convert_number(char *num);
void tarfs_print_all_files(struct tar_header *tar);
void *tarfs_get_file(struct tar_header *tar, const char *filename);
size_t tarfs_get_len(struct tar_header *tar, const char *filename);

END_DECLS

