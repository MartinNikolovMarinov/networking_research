#include "nr_bits.h"
#include "nr_logger.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct NrAllocatorStats {
    uint64_t allocationCount;
    uint64_t failedAllocationCount;
    uint64_t bytesInUse;
    uint64_t totalRequestedBytes;
    uint64_t backingBytesAllocated;
    uint64_t peekBytesInUse;
} NrAllocatorStats;

typedef struct NrStackAllocator {
    void* memory;
    uint64_t capacity;
    uint64_t offset;
    NrAllocatorStats stats;
} NrStackAllocator;

typedef struct NrArenaBlock {
    struct NrArenaBlock* next;
    NrStackAllocator stack;
} NrArenaBlock;

typedef struct NrArenaAllocator {
    NrArenaBlock* head;
    NrArenaBlock* tail;
    uint64_t defaultBlockSize;
    NrAllocatorStats stats;
} NrArenaAllocator;

NrStackAllocator nrCreateStackAllocator(uint32_t startCapacity) {
    NrStackAllocator ret = {};
    ret.capacity = startCapacity,
    ret.memory = malloc(startCapacity);
    return ret;
}

int32_t main(int32_t, const char**) {
    uint32_t v = nrAlign(8, 4);
    logInfo("v = {}", v);
    nrLogInfoToBinary(v);
    return 0;
}
