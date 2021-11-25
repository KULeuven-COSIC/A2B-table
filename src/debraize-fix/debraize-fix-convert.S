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
.syntax unified
.thumb

/* Include preprocessor directives */ 
#include "../common/A2B.h"

/* Include macros */ 
#include "../common/macros.S"

.align 4
.global A2B_convert_asm
.func A2B_convert_asm
.type A2B_convert_asm, %function
/* Register aliases */
A .req r0
R .req r1
T .req r2
beta .req r3
mod2nik .req r4 @ mod2((n - i) * k)
ik .req r5
A_l .req r6
R_l .req r7
betaBi .req r8 @ (beta || B_i)
BB .req lr @ B (symbol is reserved)
/* Offsets on stack */
.equ rrrOffs, 40
.equ clearOffs, 44
A2B_convert_asm:

    push {r4 - r12, lr} 

    clearState

    /* Trigger high */
    gpio_set 1

    mov mod2nik, #mod2nk
    mov ik, #0
    mov BB, #0

.align 4
.loop: 

        clear A_l 
        and R_l, R, #mod2k

        /* compute MSB table index */       
        orr beta, beta, ik, lsr #(log2k-1)                            
        lsl beta, beta, #k                  

        add A, A, R_l 
        and A, A, mod2nik
        and A_l, A, #mod2k
        orr A_l, A_l, beta 
        clear beta
        lsl A_l, A_l, #2 
        add A_l, T, A_l 
        ldr betaBi, [A_l] 

        flushLoad 
        flushShift 

        lsr beta, betaBi, #k
        and betaBi, betaBi, #mod2k
        eor betaBi, betaBi, R_l 
        lsl betaBi, betaBi, ik
        orr BB, BB, betaBi 

        lsr A, A, #k
        lsr mod2nik, mod2nik, #k
        lsr R, R, #k 

        add ik, ik, #k
        cmp ik, #(n*k)
        bne .loop

    mov r0, BB

    /* Trigger low */
    gpio_clear 1

    clear r1
    clear r2
    clear r3
    clearState

    pop {r4 - r12, pc}

.unreq A 
.unreq R
.unreq T
.unreq beta
.unreq mod2nik 
.unreq ik 
.unreq R_l
.unreq A_l 
.unreq betaBi
.unreq BB
.endfunc