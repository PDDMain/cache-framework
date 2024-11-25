
#include <string.h>
#include <stdio.h>
#include "cache.h"

// Simulated disk I/O functions
static int disk_read(uint64_t block_id, uint8_t* buffer) {
    // Simulate reading from disk
    FILE* f = fopen("simulated_disk.bin", "rb");
    if (!f) return CACHE_ERROR;
    
    fseek(f, block_id * BLOCK_SIZE, SEEK_SET);
    size_t read = fread(buffer, 1, BLOCK_SIZE, f);
    fclose(f);
    
    return (read == BLOCK_SIZE) ? CACHE_SUCCESS : CACHE_ERROR;
}

static int disk_write(uint64_t block_id, const uint8_t* buffer) {
    // Simulate writing to disk
    FILE* f = fopen("simulated_disk.bin", "r+b");
    if (!f) {
        f = fopen("simulated_disk.bin", "wb");
        if (!f) return CACHE_ERROR;
    }
    
    fseek(f, block_id * BLOCK_SIZE, SEEK_SET);
    size_t written = fwrite(buffer, 1, BLOCK_SIZE, f);
    fclose(f);
    
    return (written == BLOCK_SIZE) ? CACHE_SUCCESS : CACHE_ERROR;
}

cache_t* cache_init(void) {
    cache_t* cache = malloc(sizeof(cache_t));
    if (!cache) return NULL;
    
    memset(cache, 0, sizeof(cache_t));
    cache->access_counter = 0;
    return cache;
}

void cache_destroy(cache_t* cache) {
    if (!cache) return;
    
    // Flush dirty blocks before destroying
    cache_flush(cache);
    
    // Free all blocks
    for (size_t i = 0; i < cache->num_blocks; i++) {
        free(cache->blocks[i]->data);
        free(cache->blocks[i]);
    }
    
    free(cache);
}

static cache_block_t* find_block(cache_t* cache, uint64_t block_id) {
    for (size_t i = 0; i < cache->num_blocks; i++) {
        if (cache->blocks[i]->block_id == block_id) {
            return cache->blocks[i];
        }
    }
    return NULL;
}

static cache_block_t* evict_block(cache_t* cache) {
    if (cache->num_blocks == 0) return NULL;
    
    // Find least recently used block
    size_t lru_index = 0;
    uint64_t min_access = cache->blocks[0]->last_access;
    
    for (size_t i = 1; i < cache->num_blocks; i++) {
        if (cache->blocks[i]->last_access < min_access) {
            min_access = cache->blocks[i]->last_access;
            lru_index = i;
        }
    }
    
    cache_block_t* victim = cache->blocks[lru_index];
    
    // If dirty, write back to disk
    if (victim->dirty) {
        disk_write(victim->block_id, victim->data);
    }
    
    // Remove from cache
    for (size_t i = lru_index; i < cache->num_blocks - 1; i++) {
        cache->blocks[i] = cache->blocks[i + 1];
    }
    cache->num_blocks--;
    
    return victim;
}

int cache_read(cache_t* cache, uint64_t block_id, uint8_t* buffer) {
    if (!cache || !buffer) return CACHE_ERROR;
    
    cache_block_t* block = find_block(cache, block_id);
    
    if (block) {
        // Cache hit
        memcpy(buffer, block->data, BLOCK_SIZE);
        block->last_access = ++cache->access_counter;
        cache->hits++;
        return CACHE_SUCCESS;
    }
    
    // Cache miss
    cache->misses++;
    
    // If cache is full, evict a block
    if (cache->num_blocks >= MAX_CACHE_BLOCKS) {
        block = evict_block(cache);
        if (!block) return CACHE_ERROR;
    } else {
        block = malloc(sizeof(cache_block_t));
        if (!block) return CACHE_ERROR;
        block->data = malloc(BLOCK_SIZE);
        if (!block->data) {
            free(block);
            return CACHE_ERROR;
        }
    }
    
    // Read from disk
    if (disk_read(block_id, block->data) != CACHE_SUCCESS) {
        free(block->data);
        free(block);
        return CACHE_ERROR;
    }
    
    // Update block metadata
    block->block_id = block_id;
    block->last_access = ++cache->access_counter;
    block->dirty = 0;
    
    // Add to cache
    cache->blocks[cache->num_blocks++] = block;
    
    // Copy to user buffer
    memcpy(buffer, block->data, BLOCK_SIZE);
    
    return CACHE_SUCCESS;
}

int cache_write(cache_t* cache, uint64_t block_id, const uint8_t* buffer) {
    if (!cache || !buffer) return CACHE_ERROR;
    
    cache_block_t* block = find_block(cache, block_id);
    
    if (!block) {
        // Not in cache, need to allocate new block
        if (cache->num_blocks >= MAX_CACHE_BLOCKS) {
            block = evict_block(cache);
            if (!block) return CACHE_ERROR;
        } else {
            block = malloc(sizeof(cache_block_t));
            if (!block) return CACHE_ERROR;
            block->data = malloc(BLOCK_SIZE);
            if (!block->data) {
                free(block);
                return CACHE_ERROR;
            }
        }
        
        block->block_id = block_id;
        cache->blocks[cache->num_blocks++] = block;
    }
    
    // Update block data and metadata
    memcpy(block->data, buffer, BLOCK_SIZE);
    block->last_access = ++cache->access_counter;
    block->dirty = 1;
    
    return CACHE_SUCCESS;
}

int cache_flush(cache_t* cache) {
    if (!cache) return CACHE_ERROR;
    
    for (size_t i = 0; i < cache->num_blocks; i++) {
        if (cache->blocks[i]->dirty) {
            if (disk_write(cache->blocks[i]->block_id, cache->blocks[i]->data) != CACHE_SUCCESS) {
                return CACHE_ERROR;
            }
            cache->blocks[i]->dirty = 0;
        }
    }
    
    return CACHE_SUCCESS;
}

void cache_stats(cache_t* cache, size_t* hits, size_t* misses) {
    if (!cache || !hits || !misses) return;
    
    *hits = cache->hits;
    *misses = cache->misses;
}
