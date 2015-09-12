#include <stdio.h>

#include <cache.h>
#include <khash.h>

int main(int argc, char *argv[]) {
	struct cache_t *cache;

	cache = cache_new();
	if (!cache) {
		fprintf(stderr, "unable to create cache: %s\n", strerror(errno));
		return 1;
	}

	cache_add(cache, "name", "neurodrone");
	cache_add(cache, "place", "US");
	cache_add(cache, "name1", "neurodrone_");
	fprintf(stdout, "deleted value: %s\n", cache_remove(cache, "name"));

	cache_free(cache);
	return 0;
}

