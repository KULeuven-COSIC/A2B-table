#include <stdint.h>
#include <string.h>

#include "aes.h"
#include "rng.h"

extern unsigned char en_rand;
uint8_t aes_key[16] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
uint8_t aes_block[16] = {0};
aes128ctx aes_ctx;

void initRandomBytes(unsigned char *seed)
{
    aes128_ecb_keyexp(&aes_ctx, aes_key);
    for (int i = 0; i < 16; i++)
    {
        aes_block[i] = seed[i];
    }
}

void randombytes(unsigned char *x, unsigned long long xlen)
{

    if (en_rand)
    {

        while (xlen > AES_BLOCKBYTES)
        {
            aes128_ecb(aes_block, aes_block, 1, &aes_ctx);
            memcpy(x, aes_block, AES_BLOCKBYTES);
            x += AES_BLOCKBYTES;
            xlen -= AES_BLOCKBYTES;
        }

        aes128_ecb(aes_block, aes_block, 1, &aes_ctx);
        memcpy(x, aes_block, xlen);
    }
    else
    {
        memset(x, 0, xlen);
    }
}

#ifdef STMF303VC6T

uint32_t rng_get_random_blocking()
{
    uint32_t R;

    randombytes((uint8_t *)&R, 4);

    return R;
}

#endif // STMF303VC6T