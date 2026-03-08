#pragma once

#include <core.h>
#include <core_extensions/hash_functions.h>

using namespace coretypes;

enum struct RegisteredAllocators : core::AllocatorId {
    DEFAULT,

    STD_ALLOCATOR_ID,
    STD_STATS_ALLOCATOR_ID,
    LOGGER_ALLOCATOR_ID,

    SENTINEL
};
CORE_ENUM_DECLARE_INT_OPS(RegisteredAllocators, core::AllocatorId)

#define DEF_ALLOC core::getAllocator(core::DEFAULT_ALLOCATOR_ID)

void coreInit(core::LogLevel globalLogLevel);
void coreShutdown();
