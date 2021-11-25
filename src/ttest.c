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

unsigned char en_rand;

static void init(void)
{
    hal_setup();
    uint8_t seed[16] = {0};
    initRandomBytes(seed);
    en_rand = usart_recv_blocking(USART2);
    usart_send_blocking(USART2, en_rand); // small handshake
}

static int test_A2B(void)
{
    uint32_t A, R, B;

    {
        while (1)
        {
            A = 0;
            R = 0;

            /* 1. Receive inputs */
            for (size_t i = 0; i < 4; i++)
            {
                A |= ((usart_recv_blocking(USART2) & 0xff) << 8 * i);
            }

            for (size_t i = 0; i < 4; i++)
            {
                R |= ((usart_recv_blocking(USART2) & 0xff) << 8 * i);
            }

            /* 2. Convert */
            B = A2B_convert(A, R);

            /* 3. Send output */
            for (size_t i = 0; i < 4; i++)
            {
                usart_send_blocking(USART2, ((B >> 8 * i) & 0xff));
            }
        }
    }

    return 1;
}

int main(void)
{
    init();
    test_A2B();
    return 1;
}