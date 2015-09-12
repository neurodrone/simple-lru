#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "cache.h"


static void cache_free_nodes(struct cache_node_t *root);

extern struct cache_t *cache_new() {
	khash_t(str) *h = kh_init(str);

	struct cache_t *c = malloc(sizeof *c);
	if (!c) {
		return NULL;
	}

	c->root = NULL;
	c->hash = h;

	return c;
}

extern void cache_free(struct cache_t *cache) {
	cache_free_nodes(cache->root);
	kh_destroy(str, cache->hash);
}

extern int cache_add(struct cache_t *cache, const char *key, void *data) {
	int k, absent;
	struct cache_node_t *nodeval;

	k = kh_get(str, cache->hash, key);
	if (!k || (nodeval = kh_value(cache->hash, k)) == NULL) {
		nodeval = malloc(sizeof *nodeval);
		if (!nodeval) {
			errno = ENOMEM;
			return -1;
		}
		nodeval->key = key;
		nodeval->next = nodeval->prev = NULL;

		k = kh_put(str, cache->hash, key, &absent);
		kh_value(cache->hash, k) = nodeval;

	} else {
		if (nodeval->prev) {
			nodeval->prev->next = nodeval->next;
			nodeval->prev = NULL;
		}
		if (nodeval->next) {
			nodeval->next->prev = nodeval->prev;
			nodeval->next = NULL;
		}
	}

	nodeval->data = data;

	if (cache->root == NULL) {
		cache->root = nodeval;
		return 0;
	}

	cache->root->prev = nodeval;
	nodeval->next = cache->root;
	cache->root = nodeval;
	return 0;
}

extern void *cache_remove(struct cache_t *cache, const char *key) {
	int k;
	struct cache_node_t *nodeval;
	void *data;

	k = kh_get(str, cache->hash, key);
	if (!k) {
		return NULL;
	}

	nodeval = kh_value(cache->hash, k);
	assert(nodeval != NULL);

	if (nodeval->prev)
		nodeval->prev->next = nodeval->next;
	if (nodeval->next)
		nodeval->next->prev = nodeval->prev;

	if (cache->root == nodeval) {
		cache->root = nodeval->next;
		cache->root->prev = NULL;
	}

	data = nodeval->data;
	free(nodeval);

	kh_del(str, cache->hash, k);

	return data;
}

void cache_free_nodes(struct cache_node_t *root) {
	struct cache_node_t *cur = root, *next;

	while (cur) {
		next = cur->next;
		free(cur);

		cur = next;
	}
}

