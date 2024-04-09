![APM](https://badgen.net/github/license/micromatch/micromatch)
![example workflow](https://github.com/sympiler/lbc/actions/workflows/cmakeUbuntu.yml/badge.svg)
![example workflow](https://github.com/sympiler/lbc/actions/workflows/cmakeMac.yml/badge.svg)

# Aggregation

The aggregation repository contains a set of algorithms for grouping vertices of DAGs
coming from loop-carried dependencies.
Load-balance Level Coarsening (LBC) is one of the aggregation algorithms.
The algorithms in this repository can be used within code generators or libraries.

## Install

### Prerequisites

First following items should be installed:

* CMake
* C++ compiler (GCC, ICC, or CLang)
* METIS (optional) dependency for running the demo efficiently
  and is handled by the cmake. If you have installed the package using
  a packet manager (e.g., apt of homebrew), CMake should be able to detect it.
  Otherwise, it installs METIS from source internally.
* OpenMP (optional) for running some parts of the code in parallel. If you
  use GCC/ICC then OpenMP should be supported natively. If you use Apple CLang,
  you probably need to install OpenMP using `homebrew install libomp`. You can
* also install LLVM usng `brew install llvm` which support OpenMP natively.

### Build

Then build Aggregation, using the following:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

You can always set `-DCMAKE_CXX_COMPILER=` and `-DCMAKE_C_COMPILER=` to use
a different compiler. For example:
`cmake -DCMAKE_CXX_COMPILER=/usr/local/Cellar/gcc\@9/9.3.0_2/bin/g++-9  -DCMAKE_C_COMPILER=/usr/local/Cellar/gcc\@9/9.3.0_2/bin/gcc-9 ..`

## Example

The example directory shows how to call LBC API and iterate over
the created partitioning. For more examples on how LBC is used for
making loops with sparse dependencies parallel.
