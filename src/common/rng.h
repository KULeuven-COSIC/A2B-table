#ifndef RNG_H
#define RNG_H

void initRandomBytes(unsigned char *seed);
void randombytes(unsigned char *x, unsigned long long xlen);

#ifdef STMF303VC6T
uint32_t rng_get_random_blocking(void);
#else
#include <libopencm3/stm32/rng.h>
#endif

#endif
