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
#include "../common/A2B.h"
#include "../common/rng.h"
#include "../common/hal.h"

#include <string.h>

#define index(beta, A) ((beta << k) | A)

typedef struct
{
	uint32_t *T;
	uint32_t rho;
	uint32_t rrr;
	uint32_t clear; // address to clear the memory bus
} A2Bctx;

static void __attribute__((noinline)) A2B_init(A2Bctx *ctx)
{
	uint8_t r, rho;
	uint32_t rrr = 0, buff;

	buff = rng_get_random_blocking();
	rho = buff & 0x01;
	r = (buff >> 1) & mod2(k);

	/* (r || ... || r || ... || r) */
	for (size_t i = 0; i < n; i++)
	{
		rrr |= ((uint32_t)r << (i * k));
	}

	ctx->rrr = rrr;
	ctx->rho = rho;
	ctx->clear = 0;

	/* Tables */
	for (size_t A = 0; A < pow2(k); A++)
	{
		ctx->T[index(rho, A)] = (A + r) ^ ((rho << k) | r);
		ctx->T[index((rho ^ 1), A)] = (A + r + 1) ^ ((rho << k) | r);
	}
}

extern uint32_t A2B_convert_asm(uint32_t A, uint32_t R, A2Bctx ctx);
static uint32_t __attribute__((noinline)) A2B_convert_sw(uint32_t A, uint32_t R, A2Bctx ctx)
{
	uint32_t A_l, R_l, idx, mod2nik, betaBi, beta = ctx.rho, Bi, B = 0;

	mod2nik = mod2nk; // mod2((n - i) * k)

	for (size_t i = 0; i < n; i++)
	{
		R_l = R & mod2k;
		A = (A + R_l) & mod2nik;
		A_l = A & mod2k;

		betaBi = ctx.T[index(beta, A_l)];
		Bi = betaBi & mod2k;
		beta = betaBi >> k;

		Bi = Bi ^ R_l;
		B |= (Bi << i * k);

		A >>= k;
		R >>= k;
		mod2nik >>= k;
	}

	return B;
}

uint32_t A2B_convert(uint32_t A, uint32_t R)
{
	uint32_t B;

#ifdef PROFILE
	uint64_t t0, t1;
	t0 = hal_get_time();
#endif

	/* 1. Generate table */
	A2Bctx ctx;
	uint32_t T[pow2(k + 1)];
	ctx.T = T;
	A2B_init(&ctx);

#ifdef PROFILE
	t1 = hal_get_time();
	printcycles("debraize precomp cycles:", t1 - t0);
	t0 = hal_get_time();
#endif

	/* 2. Pre-mask inputs */
	A = (A - ctx.rrr) & mod2nk;

	/* 3. Convert */
#ifdef PROFILE
	B = A2B_convert_sw(A, R, ctx);
#else
	B = A2B_convert_asm(A, R, ctx);
#endif

	B = (B ^ ctx.rrr);

#ifdef PROFILE
	t1 = hal_get_time();
	printcycles("debraize conv cycles:", t1 - t0);
#endif

	/* 4. Unmask output */
	return B;
}
