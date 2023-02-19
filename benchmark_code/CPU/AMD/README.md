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

### Test different formats
To test different formats/implementations, you can use the class template provided [here](./spmv_code_bench/spmv_kernel_template.cpp). The new class inherits properties from the `Matrix_Format` class defined [here](./spmv_code_bench/spmv_kernel.h). A conversion function (`csr_to_format`) and an execution function (`spmv`) have to be defined for the new format.

### How to compile

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...). Additionally, paths for MKL, AOCL-Sparse, SparseX and SELL-C-σ libraries have to be set properly. Then you run `make` in any `spmv_code_*` directory.

(The SparseX and SELL-C-σ projects have to be built beforehand. The `install_sparsex.sh` and `install_sell_C_s.sh` bash scripts build SparseX and SELL-C-σ respectively. In these scripts, you have to specify the location of the `ROOT_DIR`, where the libraries, along with their dependencies, will be built/installed.)

### How to run

To run SpMV you simply run the `run.sh` bash script. All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
