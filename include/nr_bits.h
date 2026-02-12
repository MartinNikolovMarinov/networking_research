#ifndef NR_BITS_H
#define NR_BITS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t nrSetBitAt(uint32_t n, uint8_t at, uint8_t bit);
uint8_t nrGetBitAt(uint32_t n, uint8_t at);
void nrReverseCharArr(char* arr, int64_t len);
void nrLogInfoToBinary(uint32_t n);
bool nrIsPowerOf2(uint32_t n);
uint32_t nrAlign(uint32_t n, uint32_t alignment);

#ifdef __cplusplus
}
#endif

#endif
