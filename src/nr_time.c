#include "nr_time.h"
#include "nr_system_checks.h"

#include <stdint.h>

#if OS_WINDOWS == 1
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

#define NS_PER_SECOND 1000000000ULL

static uint64_t readMonotonicTimeNs(void) {
#if OS_WINDOWS == 1
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;

    if (!QueryPerformanceFrequency(&freq) || !QueryPerformanceCounter(&counter)) {
        return 0;
    }

    if (freq.QuadPart <= 0) {
        return 0;
    }

    uint64_t ret = 0;
    uint64_t scaledCounter = (uint64_t)counter.QuadPart * NS_PER_SECOND;
    ret = (uint64_t)(scaledCounter / (uint64_t)freq.QuadPart);
    return ret;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }

    uint64_t ret = ((uint64_t)ts.tv_sec * NS_PER_SECOND) + (uint64_t)ts.tv_nsec;
    return ret;
#endif
}

uint64_t nrGetMonotonicTime(void) {
    uint64_t ret = readMonotonicTimeNs();
    return ret;
}
