#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cache.h"

// Test patterns
#define TEST_PATTERN_A 0xAA
#define TEST_PATTERN_B 0xBB
#define TEST_PATTERN_C 0xCC

// Function to print buffer contents (for debugging)
void print_buffer(const uint8_t* buffer, size_t size) {
    printf("Buffer contents (first 32 bytes): ");
    for (size_t i = 0; i < 32 && i < size; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

// Test sequential access pattern
void test_sequential_access(cache_t* cache) {
    printf("\n=== Testing Sequential Access Pattern ===\n");
    
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    
    // Write sequential blocks
    memset(write_buffer, TEST_PATTERN_A, BLOCK_SIZE);
    for (int i = 0; i < 10; i++) {
        if (cache_write(cache, i, write_buffer) != CACHE_SUCCESS) {
            printf("Failed to write block %d\n", i);
            return;
        }
        printf("Written block %d\n", i);
    }
    
    // Read them back
    for (int i = 0; i < 10; i++) {
        if (cache_read(cache, i, read_buffer) != CACHE_SUCCESS) {
            printf("Failed to read block %d\n", i);
            return;
        }
        
        // Verify contents
        if (read_buffer[0] != TEST_PATTERN_A) {
            printf("Data mismatch in block %d\n", i);
        } else {
            printf("Successfully verified block %d\n", i);
        }
    }
    
    size_t hits, misses;
    cache_stats(cache, &hits, &misses);
    printf("Sequential access stats - Hits: %zu, Misses: %zu\n", hits, misses);
}

// Test random access pattern
void test_random_access(cache_t* cache) {
    printf("\n=== Testing Random Access Pattern ===\n");
    
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    
    // Initialize random seed
    srand(time(NULL));
    
    // Perform random accesses
    memset(write_buffer, TEST_PATTERN_B, BLOCK_SIZE);
    for (int i = 0; i < 20; i++) {
        uint64_t block_id = rand() % 100;  // Random block between 0 and 99
        
        // Write
        if (cache_write(cache, block_id, write_buffer) != CACHE_SUCCESS) {
            printf("Failed to write random block %llu\n", block_id);
            continue;
        }
        
        // Immediate read back
        if (cache_read(cache, block_id, read_buffer) != CACHE_SUCCESS) {
            printf("Failed to read random block %llu\n", block_id);
            continue;
        }
        
        // Verify contents
        if (read_buffer[0] != TEST_PATTERN_B) {
            printf("Data mismatch in random block %llu\n", block_id);
        } else {
            printf("Successfully verified random block %llu\n", block_id);
        }
    }
    
    size_t hits, misses;
    cache_stats(cache, &hits, &misses);
    printf("Random access stats - Hits: %zu, Misses: %zu\n", hits, misses);
}

// Test cache eviction
void test_cache_eviction(cache_t* cache) {
    printf("\n=== Testing Cache Eviction ===\n");
    
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    memset(write_buffer, TEST_PATTERN_C, BLOCK_SIZE);
    
    // Write more blocks than cache can hold
    printf("Writing %d blocks (more than cache capacity)...\n", MAX_CACHE_BLOCKS + 10);
    for (int i = 0; i < MAX_CACHE_BLOCKS + 10; i++) {
        if (cache_write(cache, i, write_buffer) != CACHE_SUCCESS) {
            printf("Failed to write block %d during eviction test\n", i);
            return;
        }
    }
    
    // Now read back some early blocks (should cause cache misses)
    printf("Reading back early blocks...\n");
    for (int i = 0; i < 5; i++) {
        if (cache_read(cache, i, read_buffer) != CACHE_SUCCESS) {
            printf("Failed to read block %d during eviction test\n", i);
            return;
        }
        
        if (read_buffer[0] != TEST_PATTERN_C) {
            printf("Data mismatch in evicted block %d\n", i);
        } else {
            printf("Successfully verified evicted block %d\n", i);
        }
    }
    
    size_t hits, misses;
    cache_stats(cache, &hits, &misses);
    printf("Eviction test stats - Hits: %zu, Misses: %zu\n", hits, misses);
}

// Test cache flush
void test_cache_flush(cache_t* cache) {
    printf("\n=== Testing Cache Flush ===\n");
    
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    memset(write_buffer, TEST_PATTERN_A, BLOCK_SIZE);
    
    // Write some blocks
    for (int i = 0; i < 5; i++) {
        if (cache_write(cache, i, write_buffer) != CACHE_SUCCESS) {
            printf("Failed to write block %d during flush test\n", i);
            return;
        }
    }
    
    // Flush cache
    printf("Flushing cache...\n");
    if (cache_flush(cache) != CACHE_SUCCESS) {
        printf("Cache flush failed\n");
        return;
    }
    
    // Read back and verify
    printf("Verifying blocks after flush...\n");
    for (int i = 0; i < 5; i++) {
        if (cache_read(cache, i, read_buffer) != CACHE_SUCCESS) {
            printf("Failed to read block %d after flush\n", i);
            return;
        }
        
        if (read_buffer[0] != TEST_PATTERN_A) {
            printf("Data mismatch in block %d after flush\n", i);
        } else {
            printf("Successfully verified block %d after flush\n", i);
        }
    }
}

int main() {
    printf("=== Cache Test Program ===\n");
    
    // Initialize cache
    cache_t* cache = cache_init();
    if (!cache) {
        fprintf(stderr, "Failed to initialize cache\n");
        return 1;
    }
    
    // Run tests
    test_sequential_access(cache);
    test_random_access(cache);
    test_cache_eviction(cache);
    test_cache_flush(cache);
    
    // Final statistics
    size_t hits, misses;
    cache_stats(cache, &hits, &misses);
    printf("\n=== Final Cache Statistics ===\n");
    printf("Total hits: %zu\n", hits);
    printf("Total misses: %zu\n", misses);
    printf("Hit rate: %.2f%%\n", (float)hits / (hits + misses) * 100);
    
    // Clean up
    cache_destroy(cache);
    printf("\nTests completed.\n");
    
    return 0;
}
