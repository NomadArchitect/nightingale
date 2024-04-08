#pragma once
#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct list {
	struct list *next;
	struct list *previous;
};

typedef struct list list;
typedef struct list list_node;

#define LIST_INIT(name) \
	{ &(name), &(name) }
#define LIST_DEFINE(name) list name = LIST_INIT(name)

#define container_of(type, node, ptr) \
	(type *)((char *)(ptr)-offsetof(type, node))

#define list_head(list) (list)->next

#define list_for_each(list) \
	for (list_node *it = (list)->next; it != (list); it = it->next)

#define list_for_each_safe(list) \
	for (list_node *it = (list)->next, *next = it->next; it != (list); \
		 it = next, next = it->next)

static inline void list_insert(
	struct list *before, struct list *after, struct list *new_node) {
	before->next = new_node;
	new_node->previous = before;

	new_node->next = after;
	after->previous = new_node;
}

static inline void list_append(struct list *head, struct list *new_node) {
	list_insert(head->previous, head, new_node);
}

static inline void list_prepend(struct list *head, struct list *new_node) {
	list_insert(head, head->next, new_node);
}

static inline void list_init(struct list *head) {
	head->next = head;
	head->previous = head;
}

static inline void list_remove_between(
	struct list *previous, struct list *next) {
	previous->next = next;
	next->previous = previous;
}

static inline void list_remove(struct list *node) {
	if (node->previous || node->next) {
		list_remove_between(node->previous, node->next);
	}
	list_init(node);
}

static inline bool list_empty(struct list *head) {
	return head->next == head && head->previous == head;
}

// the source list cannot be empty
static inline void list_concat(struct list *dest, struct list *source) {
	struct list *source_head = source->next;
	struct list *source_tail = source->previous;

	dest->previous->next = source_head;
	source_head->previous = dest->previous;

	source_tail->next = dest;
	dest->previous = source_tail;

	list_init(source);
}

static inline struct list *list_pop_front(struct list *head) {
	struct list *old_head = head->next;
	list_remove_between(head, old_head->next);
	list_init(old_head);
	return old_head;
}

END_DECLS

#endif // _LIST_H_
