SpMV targeting ARM CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* Altra Q80-33 processor (Ampere 'Mount Jade' server)

**Software :**

* [ARM C/C++ Compiler](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/cpp-compiler) 22.0.1
* [Arm Performance Libraries](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/performance-libraries) 22.0.1
* [SparseX](https://github.com/cslab-ntua/sparsex)
* ghost, physics, ghost-apps from [ESSEX-GHOST](https://bitbucket.org/essex/) project

### How to compile

For the SpMV benchmarks on this ARM CPU the armclang compiler was used, to obtain optimal performance. You first have to download the ARM Compiler (downloading from this [link](https://developer.arm.com/downloads/-/arm-compiler-for-linux) Performance Libraries are included). After untar-ing, install the compilers in the selected path (name it `<ARM-Compiler-Path>`).

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate ARM compiler path and benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...). Additionally, paths for SparseX and SELL-C-σ projects have to set properly. Then you run `make` in any `spmv_code_*` directory.

(The SparseX and SELL-C-σ projects have to be compiled beforehand. The `readme_sparsex.sh` bash script builds SparseX automatically. For SELL-C-σ, instructions are provided in the subdirectory.)

### How to run

To run SpMV you simply run the `run.sh` bash script. All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
