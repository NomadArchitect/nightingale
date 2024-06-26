#pragma once

#include <stdint.h>

#define ETH_MTU 1518

struct pk {
	// data must be the first field so that these can be passed
	// to the network card DMA functions directly.
	uint8_t data[ETH_MTU];

	uint16_t len;
	uint16_t l2_offset;
	uint16_t l3_offset;
	uint16_t l4_offset;

	uint16_t l2_type;
	uint16_t l3_type;
	uint16_t l4_type;

	// The interface on which the pk was received, null if it was
	// generated locally.
	struct net_if *origin_if;

	// The next pk containing data for the same frame.
	struct pk *next_in_frame;

	// The next pk on a queue or list, including the free list.
	struct pk *queue_next;
};

#define L2(x) (void *)((x)->data + (x)->l2_offset)
#define L3(x) (void *)((x)->data + (x)->l3_offset)
#define L4(x) (void *)((x)->data + (x)->l4_offset)

static_assert(sizeof(struct pk) <= 2048, "pk is too large");

struct pk *pk_alloc();
void pk_free(struct pk *pk);

void pk_drop(struct pk *pk);
void pk_reject(struct pk *pk);
void pk_done(struct pk *pk);
