#ifndef CACHE_H
#define CACHE_H

#include "khash.h"

KHASH_MAP_INIT_STR(str, struct cache_node_t *)

struct cache_t {
	struct cache_node_t *root;
	khash_t(str) *hash;
};

struct cache_node_t {
	struct cache_node_t *prev, *next;
	const char *key;
	void *data;
};

struct cache_t *cache_new();

void cache_free(struct cache_t *cache);

int cache_add(struct cache_t *cache, const char *key, void *data);

void *cache_remove(struct cache_t *cache, const char *key);

void *cache_get(struct cache_t *cache, const char *key);

#endif
