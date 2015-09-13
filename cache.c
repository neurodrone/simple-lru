#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "cache.h"


extern struct cache_t *cache_new(size_t capacity) {
	khash_t(str) *h = kh_init(str);

	struct cache_t *c = malloc(sizeof *c);
	if (!c) {
		errno = ENOMEM;
		return NULL;
	}

	c->buf = calloc(capacity, sizeof *c->head);
	if (!c->head) {
		errno = ENOMEM;
		return NULL;
	}
	c->head = c->tail = c->buf[0];
	c->len  = 0;
	c->capacity = capacity;
	c->hash = h;

	return c;
}

extern void cache_free(struct cache_t *cache) {
	free(cache->buf);
	kh_destroy(str, cache->hash);
	free(cache);
}

extern int cache_add(struct cache_t *cache, const char *key, void *data) {
	int k, absent;
	struct cache_node_t *nodeval;

	if (cache->tail == cache->head && cache->head) {
		cache_evict(cache);
	}

	k = kh_get(str, cache->hash, key);
	if (!k || (nodeval = kh_value(cache->hash, k)) == NULL) {
		nodeval = malloc(sizeof *nodeval);
		if (!nodeval) {
			errno = ENOMEM;
			return -1;
		}
		nodeval->key = key;

		k = kh_put(str, cache->hash, key, &absent);
		kh_value(cache->hash, k) = nodeval;
	}

	nodeval->data = data;

	cache->tail = nodeval;

	if (cache->tail == cache->buf[cache->capacity - 1]) {
		cache->tail = cache->buf[0];
	} else {
		cache->tail++;
	}

	cache->len++;

	return 0;
}

extern void *cache_get(struct cache_t *cache, const char *key) {
	int k;
	struct cache_node_t *nodeval, *tmp;

	k = kh_get(str, cache->hash, key);
	if (!k) {
		return NULL;
	}

	nodeval = kh_value(cache->hash, k);
	assert(nodeval != NULL);

	return nodeval->data;
}

extern void *cache_evict(struct cache_t *cache) {
	int k;
	void *data;
	struct cache_node_t *nodeevict, **ptrevict;

	if (cache->len == 0) {
		return NULL;
	}

	nodeevict = cache->head;
	ptrevict = &nodeevict;

	if (cache->head == cache->buf[cache->capacity - 1]) {
		cache->head = cache->buf[0];
	} else {
		cache->head++;
	}

	data = nodeevict->data;

	k = kh_get(str, cache->hash, nodeevict->key);
	assert(k != 0);

	kh_del(str, cache->hash, k);
	*ptrevict = NULL;

	cache->len--;

	return data;
}
