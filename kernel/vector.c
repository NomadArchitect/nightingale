
#include <panic.h>
// #include <stdio.h> // Temporary
#include <print.h>
// #include <stdlib.h> // Temporary
#include <malloc.h>
#include <string.h>

#include "vector.h"

Vector *new_vec_internal(const char *type, size_t count, size_t delta, Strategy strategy) {
    Vector *result = malloc(sizeof(Vector));

    result->type = type;
    result->len = 0;
    result->total_size = count;
    result->delta = delta;
    result->strategy = strategy;
    result->data = malloc(count * delta);

    return result;
}

void vec_set(Vector *vec, size_t index, void *value) {
    assert(vec->total_size > vec->len, "Set out-of-bounds");

    memcpy((vec->data + (vec->delta * index)), value, vec->delta);
}

void vec_push(Vector *vec, void *value) {
    void *new_data;
    size_t new_len = 0;

    if (vec->total_size > vec->len) {
        vec_set(vec, vec->len, value);
        vec->len += 1;
    } else {
        // printf("Decided to allocate, from %zu ", vec->total_size);
        switch (vec->strategy) {
        case STRATEGY_BY_1:
            new_len = vec->total_size + 1;
            break;
        case STRATEGY_BY_16:
            new_len = vec->total_size + 16;
            break;
        case STRATEGY_BY_256:
            new_len = vec->total_size + 256;
            break;
        case STRATEGY_X2:
            new_len = vec->total_size * 2;
            break;
        case STRATEGY_X4:
            new_len = vec->total_size * 4;
            break;
        case STRATEGY_PHI:
            new_len = vec->total_size * 3 / 2; // Most memory efficient theoretically is  *phi
            break;
        default:
            printf("strategy %i unknown - using default\n", vec->strategy);
            vec->strategy = STRATEGY_PHI; // Therefore, phi should be the default
            vec_push(vec, value);
        }

        new_data = realloc(vec->data, new_len  *vec->delta);

        // printf("to %zu\n", new_len);

        assert(new_data != NULL,   "Reallocating to up buffer failed - "
                                   "consider a different strategy\n");

        vec->total_size = new_len;
        vec->data = new_data;
        vec_push(vec, value);
    }
}

void *vec_get(Vector *vec, size_t index) {
    assert(vec->len > index, "Access out-of-bounds");

    return vec->data + (vec->delta * index);
}

const char *vec_strats[] = {
    "by 1",
    "by 16",
    "by 256",
    "phi",
    "x2",
    "x4"
};

void print_vector(Vector *v) {
    printf("Vector<%s> @%zx{len=%zu, total_size=%zu, strategy=%s, data=%zx}\n", 
        v->type, v, v->len, v->total_size, vec_strats[v->strategy], v->data);
}

