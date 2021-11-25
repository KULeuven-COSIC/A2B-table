/*
 * MIT License
 *
 * Copyright (c) 2021: imec-COSIC KU Leuven, 3001 Leuven, Belgium 
 * Author: Michiel Van Beirendonck <michiel.vanbeirendonck@esat.kuleuven.be>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common/hal.h"
#include "common/rng.h"
#include "common/A2B.h"

#include <libopencm3/stm32/usart.h>

const size_t NTEST = 100;
unsigned char en_rand;

static void init(void)
{
    hal_setup();
    uint8_t seed[16] = {0};
    initRandomBytes(seed);
    en_rand = 1;
}

static int test_A2B(void)
{
    uint32_t A, R, B;
    uint8_t buff[8];

    hal_send_str("==========================");

    {

        for (size_t i = 0; i < NTEST; i++)
        {

            /* 1. Receive inputs */
            randombytes((uint8_t *)&A, 4);
            A &= mod2nk;

            randombytes((uint8_t *)&R, 4);
            R &= mod2nk;

            /* 2. Convert */
            B = A2B_convert(A, R);

            /* 3. Send output */
            if (((A + R) & mod2nk) == (B ^ R))
            {
                hal_send_str("OK A2B\n");
            }
            else
            {
                hal_send_str("ERROR A2B\n");
                return 1;
            }
        }
    }

    hal_send_str("============ ALL A2B OK ==============");
    return 0;
}

int main(void)
{
    init();
    test_A2B();
    return 0;
}