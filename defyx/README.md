
# DefyX
DefyX is a proof-of-work (PoW) algorithm that is optimized for general-purpose CPUs. DefyX uses random code execution (hence the name) together with several memory-hard techniques to minimize the efficiency advantage of specialized hardware.

## Overview

DefyX utilizes a virtual machine that executes programs in a special instruction set that consists of integer math, floating point math and branches. These programs can be translated into the CPU's native machine code on the fly (example: [program.asm](doc/program.asm)). At the end, the outputs of the executed programs are consolidated into a 256-bit result using a cryptographic hashing function ([Blake2b](https://blake2.net/), YescryptRH and also K12.

DefyX can operate in two main modes with different memory requirements:

* **Fast mode** - requires more memory.
* **Light mode** - requires less memory but also runs much slower.

Both modes are interchangeable as they give the same results. The fast mode is suitable for "mining", while the light mode is expected to be used only for proof verification.

## Build

DefyX is written in C++11 and builds a static library with a C API provided by header file [defyx.h](src/defyx.h). Minimal API usage example is provided in [api-example1.c](src/tests/api-example1.c). The reference code includes a `defyx-benchmark` and `defyx-tests` executables for testing.

### Linux

Build dependencies: `cmake` (minimum 2.8.7) and `gcc` (minimum version 4.8, but version 7+ is recommended).

To build optimized binaries for your machine, run:
```
git clone https://github.com/tevador/DefyX.git
cd DefyX
mkdir build && cd build
cmake -DARCH=native ..
make
```
### Windows

On Windows, it is possible to build using MinGW (same procedure as on Linux) or using Visual Studio 2017 (solution file is provided).

## Proof of work

DefyX was primarily designed as a PoW algorithm for [Scala](https://www.scalaproject.io). 

### CPU performance

### Will be updated later. 


# FAQ

### Which CPU is best for mining DefyX?

Most Intel and AMD CPUs made since 2011 should be fairly efficient at DefyX. More specifically, efficient mining requires:

* 64-bit architecture
* IEEE 754 compliant floating point unit
* Hardware AES support ([AES-NI](https://en.wikipedia.org/wiki/AES_instruction_set) extension for x86, Cryptography extensions for ARMv8)
* 16 KiB of L1 cache, 256 KiB of L2 cache and 2 MiB of L3 cache per mining thread
* Support for large memory pages

### Since DefyX uses floating point math, does it give reproducible results on different platforms?

DefyX uses only operations that are guaranteed to give correctly rounded results by the [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) standard: addition, subtraction, multiplication, division and square root. Special care is taken to avoid corner cases such as NaN values or denormals.


## Acknowledgements
* [SChernykh](https://github.com/SChernykh) - contributed significantly to the design of DefyX
* [hyc](https://github.com/hyc) - original idea of using random code execution for PoW
* Other contributors: [nioroso-x3](https://github.com/nioroso-x3), [jtgrassie](https://github.com/jtgrassie)
* Colin Percival and Alexander Peslyak - Implemeted the yescrypt code
* Ronny Van Keer - Implemented the K12 code.

DefyX uses some source code from the following 3rd party repositories:
* Argon2d, Blake2b hashing functions: https://github.com/P-H-C/phc-winner-argon2

The author of DefyX declares no competing financial interest in DefyX adoption, other than being a holder of Monero. The development of DefyX was funded from the author's own pocket with only the help listed above.

## Donations

If you'd like to use DefyX, please consider donating to help cover the development cost of the algorithm.

Author's XMR address:
```
845xHUh5GvfHwc2R8DVJCE7BT2sd4YEcmjG8GNSdmeNsP5DTEjXd1CNgxTcjHjiFuthRHAoVEJjM7GyKzQKLJtbd56xbh7V
```
Total donations received: ~3.74 XMR (as of 9th July 2019). Thanks to all contributors.
