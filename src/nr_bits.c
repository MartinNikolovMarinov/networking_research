#include "nr_bits.h"
#include "nr_logger.h"

#include <assert.h>
#include <limits.h>

uint32_t nrSetBitAt(uint32_t n, uint8_t at, uint8_t bit) {
    if (at >= 32u) {
        return n;
    }

    uint32_t mask = 1u << at;
    uint32_t bitValue = ((uint32_t)bit & 1u) << at;
    uint32_t ret = (n & ~mask) | bitValue;
    return ret;
}

uint8_t nrGetBitAt(uint32_t n, uint8_t at) {
    if (at >= 32u) {
        return 0u;
    }

    uint8_t ret = (uint8_t)((n >> at) & 1u);
    return ret;
}

void nrReverseCharArr(char* arr, int64_t len) {
    if (arr == NULL || len <= 1) {
        return;
    }

    for (int64_t i = 0; i < len / 2; ++i) {
        char tmp = arr[i];
        arr[i] = arr[len - 1 - i];
        arr[len - 1 - i] = tmp;
    }
}

void nrLogInfoToBinary(uint32_t n) {
    if (n == 0u) {
        logInfo("bin_v = 0b0");
        return;
    }

    char buf[35];
    int32_t widx = 0;
    while (n > 0u) {
        buf[widx++] = (n & 1u) != 0u ? '1' : '0';
        n = n >> 1u;
    }

    buf[widx++] = 'b';
    buf[widx++] = '0';
    buf[widx] = '\0';

    nrReverseCharArr(buf, widx);

    int32_t bitCount = widx - 2;
    logInfo("bin_v = {} ({})", buf, bitCount);
}

bool nrIsPowerOf2(uint32_t n) {
    if (n == 0u) {
        return false;
    }

    bool ret = (n & (n - 1u)) == 0u;
    return ret;
}

uint32_t nrAlign(uint32_t n, uint32_t alignment) {
    assert(alignment != 0u);
    assert(nrIsPowerOf2(alignment));

    if (alignment == 0u || !nrIsPowerOf2(alignment)) {
        return n;
    }

    uint32_t addend = alignment - 1u;
    assert(n <= UINT32_MAX - addend);
    if (n > UINT32_MAX - addend) {
        return 0u;
    }

    uint32_t mask = ~addend;
    uint32_t ret = (n + addend) & mask;
    return ret;
}
