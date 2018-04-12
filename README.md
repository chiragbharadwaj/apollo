# apollo

**Uncovering Sources of Speedup for Performance Improvement in Specialized Hardware Accelerators**

### Prerequisites

This project requires the following tools:

 + A `clang`/LLVM toolchain, 5.0.0+
 + `cmake` 3.10+
 + `ninja` 1.8.2 (optional)

### Usage

To compile the LLVM pass, navigate to the root directory of the project. To compile the files, run

    cmake -G Ninja

Alternatively, run it without the flags to target `Makefile` instead. Then, a simple run of

    ninja
  
(or `make`) as needed will generate the pass in the `lib` directory. One can then run the pass on various files using the following syntax (assuming access from the root of the project):

    opt -load lib/PASS.so -FLAG < PATH_TO_SRC_FILE
  
Obviously, this is not a very good pass right now, but we hope to make it better. To generate the bitcode (`.bc`) file, one can use `clang` or `clang++`:

    clang++ -std=c++14 -emit-llvm -c SRC_FILE.cpp
