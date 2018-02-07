
#include <basic.h>

#define DEBUG
#include <debug.h>
#include <panic.h>

#include "pmm.h"
#include "malloc.h"

#define MINIMUM_BLOCK 32

void *current_position;

typedef struct MBlock {
    usize len;
    bool is_free;
    struct MBlock *next;
} MBlock;

MBlock *init = (void *)0x1c0000;

void init_heap(usize start) {
    init->len = -1;
    init->is_free = true;
    init->next = NULL;
}

void *malloc(usize s) {

    /* round s to a multiple of 8 bytes. */
    s = s + 7 & ~7;

    DEBUG_PRINTF("malloc(%i)\n", s);

    MBlock *cur;
    for (cur = init; !(cur->is_free) || s > cur->len; cur = cur->next) {
        if (cur->next == NULL) {
            /* The last block in the last does not have space for us. */
            return NULL;
        }
    }
    /* cur is now a block we can use */

    //DEBUG_PRINTF("We can use %x!\n", cur);

    /* try to see if we have space to cut it up into smaller blocks */
    if (cur->len > s + sizeof(MBlock) + MINIMUM_BLOCK) {
        cur->len = s;
        MBlock *tmp = cur->next;

        /* pointer arithmetic is C is + n * sizeof(*ptr) - I have to hack it to an i32 for this */
        /* next = current + header_len + allocation */
        cur->next = (MBlock *)((usize)cur + s + sizeof(MBlock));

        cur->next->len = cur->len - s - sizeof(MBlock);
        cur->next->is_free = true;
        cur->next->next = tmp;

        cur->len = s;
        cur->is_free = false;
        return (void *)(cur) + sizeof(MBlock);
    } else {
        cur->is_free = false;
        return (void *)(cur) + sizeof(MBlock);
    }

    WARN_PRINTF("error: malloc should never get here!\n");
    return NULL;
}

void free(void *v) {
    /* This is wildly unsafe - I just take you at your word that this was allocated.
     * Please don't break my trust ;-; */

    DEBUG_PRINTF("free(%x)\n", v);

    MBlock *cur = (MBlock *)(v - sizeof(MBlock));
    cur->is_free = true;
}

