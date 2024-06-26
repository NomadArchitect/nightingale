#pragma once

#include "sys/cdefs.h"
#include <stdint.h>
#include <stdlib.h>

BEGIN_DECLS

struct chacha20_state {
	uint32_t n[16];
};

struct chacha20_state chacha20_init(
	const char key[static 32], const char nonce[static 12], uint32_t count);

void chacha20_read(struct chacha20_state *state, char *buffer, size_t len);

END_DECLS
