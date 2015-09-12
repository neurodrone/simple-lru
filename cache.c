#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "khash.h"

KHASH_MAP_INIT_STR(str, struct cache_node_t *)

typedef void* hash_t;

struct cache_t {
	struct cache_node_t *root;
	khash_t(str) *hash;
};

struct cache_node_t {
	struct cache_node_t *prev, *next;
	const char *key;
	void *data;
};

void cache_free_nodes(struct cache_node_t *root) {
	struct cache_node_t *cur = root, *next;

	while (cur) {
		next = cur->next;
		free(cur);

		cur = next;
	}
}

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

		k = kh_put(str, cache->hash, key, &absent);
		kh_value(cache->hash, k) = nodeval;

	} else {
		if (nodeval->next)
			nodeval->next->prev = nodeval->prev;
		if (nodeval->prev)
			nodeval->prev->next = nodeval->next;
	}

	nodeval->data = data;

	if (cache->root == NULL) {
		cache->root = malloc(sizeof *cache->root);
		if (!cache->root) {
			errno = ENOMEM;
			return -1;
		}
		cache->root = nodeval;
		return 0;
	}

	cache->root->prev = nodeval;
	nodeval->next = cache->root;
	cache->root = nodeval;
	return 0;
}

extern void print_cache(struct cache_t *cache, FILE *f) {
	struct cache_node_t *node = cache->root;

	if (!f) {
		f = stdout;
	}

	while (node) {
		fprintf(f, "Node: %s, Value: %s\n", node->key, node->data);
		node = node->next;
	}
}

int main(int argc, char *argv[]) {
	struct cache_t *cache;

	cache = cache_new();
	if (!cache) {
		fprintf(stderr, "unable to create cache: %s\n", strerror(errno));
		return 1;
	}

	print_cache(cache, stdout);

	cache_free(cache);
}
