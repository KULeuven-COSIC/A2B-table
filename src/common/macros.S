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

/* Set osci trigger */ 
.macro gpio_set gpio

	ldr r11, =0x48000000 @ GPIOA : =0x48000000 for STMF3, =0x40020000 for STMF4
    mov r12, #(1 << \gpio)
    str r12, [r11, #24]

.endm

/* Clear osci trigger */ 
.macro gpio_clear gpio

	ldr r11, =0x48000000 @ GPIOA 
    mov r12, #((1 << \gpio) << 16)
    str r12, [r11, #24]

.endm

/* Clear register for Hamming-Distance (HD) leakage */ 
.macro clear reg 

	mov \reg, #0 

.endm

/* Clear memory load bus for HD leakage */ 
.macro flushLoad

	ldr r11, [sp, #clearOffs]

.endm

/* Flush barrel shifter for HD leakage */ 
.macro flushShift

	lsr r11, r11, #4 
    lsl r12, r12, #12

.endm

/* Flush ALU for HD leakage */ 
.macro flushALU

	add r12, r12, r11 
	sub r12, r12, r11
	and r12, r12, r11 
	eor r12, r12, r11 
    orr r12, r12, r11

.endm

/* Clear microarchitectural state */ 
.macro clearState

    clear r4
    clear r5
    clear r6
    clear r7
    clear r8
    clear r9
    clear r10
    clear r11
    clear r12
    flushLoad
    flushShift
    flushALU

.endm