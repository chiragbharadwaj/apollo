# apollo

**Uncovering Sources of Speedup for Performance Improvement in Specialized Hardware Accelerators**

### Prerequisites

This project was built with the following tools:

 + GNU Make 4.1
 + `cmake` 3.11.4
 + `ninja` 1.8.2
 + `clang`/LLVM toolchain, 7.0.0+ (with C++14 support)

No compatibility is guaranteed for other compilers/versions of these toolchains, mainly because LLVM is fairly backwards-incompatible. :-)

### Usage

To compile the LLVM pass, navigate to the root directory of the project. To compile the files, simply run

    make

to generate compilation files in the `build` directory. Then, a simple run of

    ninja -C build

in the root directory will compile the files. <location TBD>

### Sources

We used the following sources to aid in our design of this tool:

1. [https://github.com/smanilov/icsa-dswp](https://github.com/smanilov/icsa-dswp)

2. [https://github.com/ysshao/ALADDIN](https://github.com/ysshao/ALADDIN)

3. [https://github.com/compor/llvm-ir-cmake-utils](https://github.com/compor/llvm-ir-cmake-utils)

4. [https://github.com/S2E/tools/blob/master/LLVMBitcode.cmake](https://github.com/S2E/tools/blob/master/LLVMBitcode.cmake)

5. J. Ferrante, K. J. Ottenstein, and J. D. Warren, "The program dependence graph and its use in optimization," in *Symposium on Programming*, 1984.
