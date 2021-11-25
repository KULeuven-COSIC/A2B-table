# Analysis and Comparison of Table-based Arithmetic to Boolean Masking

This repository contains ARM Cortex-M4 code for first-order table-based Arithmetic to Boolean (A2B) masking conversions. The implementations are described in our paper "Analysis and Comparison of Table-based Arithmetic to Boolean Masking", Michiel Van Beirendonck and Jan-Pieter D’Anvers and Ingrid Verbauwhede [[ePrint]](https://eprint.iacr.org/2021/067) that appeared in TCHES, Volume 2021, Issue 3 [[TCHES]](https://tches.iacr.org/index.php/TCHES/article/view/8975).

## Requirements

Building and running the code requires:

* [arm-none-eabi toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
* [stlink](https://github.com/stlink-org/stlink)
* `python3` with `pyserial`

### libopencm3

The code in this repository uses the [libopencm3](https://github.com/libopencm3/libopencm3) open-source ARM Cortex-M microcontroller library. The repository is built after the [libopencm3-template](https://github.com/libopencm3/libopencm3-template).

After cloning or downloading this repository, it is necessary to initialize [libopencm3](https://github.com/libopencm3/libopencm3):

```bash
git submodule update --init --recursive
make -C libopencm3
```

## Tests and Benchmarks

### Setup

The [Makefile](./Makefile) allows to configure which benchmarks or tests to run.

It is possible to switch between the three algorithms described in the paper:

```make
ALGORITHM = {debraize, debraize-fix, dual-lookup}
```

The algorithms can be [profiled](./src/profile.c) or subjected to a [t-test](./src/ttest.c):

```make
TYPE = {profile, ttest}
```

We assume that profiling is performed on the popular [STM32F4 Discovery board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html), whereas for the t-test we used a custom PCB with an STMF303VC6T chip. If needed, this can be adjusted to your setup in [`hal.c`](./src/common/hal.c).

Finally, we allow different values to be set for the bit-size parameter `k`:

```make
CFLAGS += {-DK4, -DK8}
```

### Run

To run the [profiling benchmarks](./src/profile.c) and view the output, first connect to the board over the USB-serial interface:

```bash
make screen
```

Then compile and flash the code onto the board:

```bash
make flash
```

The [t-test executable](./src/ttest.c) can be flashed similarly, but expects to receive fixed-vs-random inputs over the serial interface from a host-side script.

## Bibliography

If you use or build upon the code in this repository, please cite our paper using our [citation key](./CITATION).

---
Michiel Van Beirendonck and Jan-Pieter D’Anvers and Ingrid Verbauwhede
