# simple-lru
A nifty LRU cache in C.

### Installation

Running `make` from the highest level within this directory should
result in a generation of `libcache.a` that contains the necessary
`simplecache` symbols to use.

```bash
$ make
```

### Usage

Build your code using `-lcache` to access the symbols. Check
[`cache.h`](cache.h) for the list of exported functions.
