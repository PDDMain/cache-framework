
#ifndef CACHE_FRAMEWORK_H
#define CACHE_FRAMEWORK_H

#include <stdint.h>
#include <stdlib.h>

#define CACHE_SUCCESS 0
#define CACHE_ERROR -1
#define MAX_CACHE_BLOCKS 1024
#define BLOCK_SIZE 4096

typedef struct {
    uint64_t block_id;
    uint8_t* data;
    uint64_t last_access;
    int dirty;
} cache_block_t;

typedef struct {
    cache_block_t* blocks[MAX_CACHE_BLOCKS];
    size_t num_blocks;
    uint64_t access_counter;
    size_t hits;
    size_t misses;
} cache_t;

// Initialize the cache
cache_t* cache_init(void);

// Clean up the cache
void cache_destroy(cache_t* cache);

// Read a block through the cache
int cache_read(cache_t* cache, uint64_t block_id, uint8_t* buffer);

// Write a block through the cache
int cache_write(cache_t* cache, uint64_t block_id, const uint8_t* buffer);

// Flush dirty blocks to disk
int cache_flush(cache_t* cache);

// Get cache statistics
void cache_stats(cache_t* cache, size_t* hits, size_t* misses);

#endif /* CACHE_FRAMEWORK_H */