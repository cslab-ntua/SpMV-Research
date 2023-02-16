SpMV targeting x86-64 CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* AMD EPYC 7402 24-Core Processor
* Intel Xeon Gold 5120 CPU
* (...any other x86-64 CPU can be tested too...)

**Software :**

* gcc/g++ compiler (benchmarks were tested successfully on versions 7.5.0 & 10.2.1)
* [AOCL-Sparse Library](https://www.amd.com/en/developer/aocl/sparse.html) 3.2 (for AMD Systems)
* [Intel MKL library](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html) (for Intel and AMD Systems)
* [SparseX](https://github.com/cslab-ntua/sparsex)
* ghost, physics, ghost-apps from [ESSEX-GHOST](https://bitbucket.org/essex/) project

### How to compile

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...). Additionally, paths for MKL, AOCL-Sparse, SparseX and SELL-C-σ libraries have to set properly. Then you run `make` in any `spmv_code_*` directory.

(The SparseX and SELL-C-σ projects have to be compiled beforehand. The `readme_sparsex.sh` bash script builds SparseX automatically. For SELL-C-σ, instructions are provided in the subdirectory.)

### How to run

To run SpMV you simply run the `run.sh` bash script. All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
