SpMV targeting AMD EPYC CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* AMD EPYC 7742 64-Core Processor

**Software :**

* [Intel Math Kernel Library (Intel MKL)](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html) 20-19.1.0
* [CSR5 (CPU AVX2 version)](https://github.com/weifengliu-ssslab/Benchmark_SpMV_using_CSR5)

### How to compile

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate library paths and benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...) and then you run `make` in both the `spmv_code_csr5` and `spmv_code_mkl-naive` directories.

### How to run

To run the SpMV benchmarks you simply run the `run.sh` bash script file.
All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
