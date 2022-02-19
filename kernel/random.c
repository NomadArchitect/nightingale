#include <basic.h>
#include <stdatomic.h>
#include <string.h>
#include <x86/cpu.h>
#include "chacha20.h"

#define POOL_BITS 8
#define POOL_SIZE (1 << POOL_BITS)
#define POOL_MASK (POOL_SIZE - 1)

char random_pool[256];

void add_to_random(const char *buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        random_pool[i & POOL_MASK] += buffer[i];
    }

    struct chacha20_state state = {{
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
    }};

    memcpy(state.n + 4, buffer, min(len, 48));

    char buf[64];
    for (int i = 0; i < 4; i++) {
        chacha20_keystream(&state, buf, 64);
        for (int j = 0; j < 64; j++) {
            random_pool[j + i * 64] ^= buf[j];
        }
    }
}

void random_dance() {
    for (int i = 0; i < 100; i++) {
        uint64_t time = rdtsc();
        add_to_random((char *)&time, 8);
    }
}

atomic_long global_nonce = 1;

size_t get_random(char *buffer, size_t len) {
    long nonce[2] = {0, 1};
    nonce[0] = atomic_fetch_add(&global_nonce, 1);
    struct chacha20_state state = init(
            random_pool,
            (char *)nonce,
            1
    );

    chacha20_keystream(&state, buffer, len);
    return len;
}
