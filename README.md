# Block Cache Framework

A lightweight, efficient block caching framework implemented in C, designed for testing and experimenting with different caching strategies. This framework provides a simple interface for caching block-level read and write operations with an LRU (Least Recently Used) eviction policy.

## Features

- **LRU Eviction Policy**: Automatically manages cache capacity using LRU strategy
- **Write-back Caching**: Reduces disk writes by only writing dirty blocks when necessary
- **Configurable Cache Size**: Easy to adjust cache and block sizes
- **Performance Monitoring**: Built-in hit/miss statistics
- **Error Handling**: Comprehensive error checking and reporting
- **Test Suite**: Includes comprehensive tests for various access patterns
- **Simulated Disk I/O**: Includes simulated disk operations that can be replaced with real I/O

## Technical Specifications

- Default cache size: 1024 blocks
- Default block size: 4096 bytes (4KB)
- Implementation language: C
- Platform: Cross-platform compatible
- Dependencies: Standard C library only

## Project Structure

```
.
├── cache_framework.h    # Header file with public API declarations
├── cache_framework.c    # Implementation of cache operations
├── main.c              # Test program and usage examples
└── README.md           # This file
```

## API Reference

### Core Functions

```c
// Initialize a new cache instance
cache_t* cache_init(void);

// Clean up and destroy cache instance
void cache_destroy(cache_t* cache);

// Read a block through the cache
int cache_read(cache_t* cache, uint64_t block_id, uint8_t* buffer);

// Write a block through the cache
int cache_write(cache_t* cache, uint64_t block_id, const uint8_t* buffer);

// Flush all dirty blocks to disk
int cache_flush(cache_t* cache);

// Get cache statistics
void cache_stats(cache_t* cache, size_t* hits, size_t* misses);
```

### Return Values

- `CACHE_SUCCESS` (0): Operation completed successfully
- `CACHE_ERROR` (-1): Operation failed

## Building and Testing

### Prerequisites

- C compiler (GCC recommended)
- Make (optional, for building using Makefile)
- Standard C library

### Building

```bash
# Using GCC directly
gcc -o cache_test cache_framework.c main.c

# Or using Makefile (if provided)
make
```

### Running Tests

```bash
./cache_test
```

The test program will run a series of tests including:
1. Sequential access patterns
2. Random access patterns
3. Cache eviction scenarios
4. Cache flush operations

## Usage Example

```c
#include "cache_framework.h"

int main() {
    // Initialize cache
    cache_t* cache = cache_init();
    if (!cache) {
        fprintf(stderr, "Failed to initialize cache\n");
        return 1;
    }

    // Prepare test data
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    memset(write_buffer, 'A', BLOCK_SIZE);

    // Write to cache
    if (cache_write(cache, 0, write_buffer) != CACHE_SUCCESS) {
        fprintf(stderr, "Write failed\n");
        return 1;
    }

    // Read from cache
    if (cache_read(cache, 0, read_buffer) != CACHE_SUCCESS) {
        fprintf(stderr, "Read failed\n");
        return 1;
    }

    // Get cache statistics
    size_t hits, misses;
    cache_stats(cache, &hits, &misses);
    printf("Cache hits: %zu, misses: %zu\n", hits, misses);

    // Clean up
    cache_destroy(cache);
    return 0;
}
```

