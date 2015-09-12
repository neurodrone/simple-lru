CFLAGS=-c -pedantic -Wall

all: simplecache

simplecache: libcache.a

libcache.a: cache.o
	ar rcs $@ $^

cache.o: cache.c khash.h
	$(CC) $(CFLAGS) cache.c
clean:
	rm *.o
