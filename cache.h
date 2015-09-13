#ifndef CACHE_H
#define CACHE_H

#include "khash.h"

KHASH_MAP_INIT_STR(str, struct cache_node_t *)

struct cache_t {
	struct cache_node_t *head, *tail;
	struct cache_node_t **buf;
	khash_t(str) *hash;

	size_t len, capacity;
};

struct cache_node_t {
	const char *key;
	void *data;
};

struct cache_t *cache_new();

void cache_free(struct cache_t *cache);

int cache_add(struct cache_t *cache, const char *key, void *data);

void *cache_evict(struct cache_t *cache);

#endif
