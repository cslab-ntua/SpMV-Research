SpMV targeting ARM CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* Altra Q80-33 processor (Ampere 'Mount Jade' server)
* NVIDIA GH200 Grace Hopper Superchip (SuperMicro 'ARS-111GL-NHR' server)

**Software :**

* [ARM C/C++ Compiler](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/cpp-compiler) 24.04)
* [Arm Performance Libraries](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/performance-libraries) 24.04
* [SparseX](https://github.com/cslab-ntua/sparsex)
* ghost, physics, ghost-apps from [ESSEX-GHOST](https://bitbucket.org/essex/) project
* (optional for Ampere server) Kernel version 5.10.27 from [ampere-lts-kernel](https://github.com/AmpereComputing/ampere-lts-kernel/tree/linux-5.10.y) (for power measurements)

Note: Power measurements are for now disabled. It will be fixed in later versions.

### Test different formats
To test different formats/implementations, you can use the class template provided [here](./spmv_code_bench/spmv_kernel_template.cpp). The new class inherits properties from the `Matrix_Format` class defined [here](./spmv_code_bench/spmv_kernel.h). A conversion function (`csr_to_format`) and an execution function (`spmv`) have to be defined for the new format.

### How to compile

For the SpMV benchmarks on this ARM CPU the armclang compiler was used, to obtain optimal performance. You first have to download the ARM Compiler (downloading from this [link](https://developer.arm.com/downloads/-/arm-compiler-for-linux) Performance Libraries are included). After untar-ing, install the compilers in the selected path (name it `<ARM-Compiler-Path>`).

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate ARM compiler path and benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...). Additionally, paths for SparseX and SELL-C-σ projects have to be set properly. Then you run `make` in any `spmv_code_*` directory.

The SparseX and SELL-C-σ projects have to be built beforehand. The `install_sparsex.sh` and `install_sell_C_s.sh` bash scripts build SparseX and SELL-C-σ respectively. In these scripts, you have to specify the location of the `ROOT_DIR`, where the libraries, along with their dependencies, will be built/installed.

### How to run

To run SpMV you simply run the `run.sh` bash script. All the configuration needed is in the `config.sh` file, along with documentation comments for each option.

Note: SELL-C-σ is not currently running on GraceHopper system. It recognizes 8 NUMA nodes, while there is only 1 for the CPU. The other are GPU MIG instances. It will be fixed in later versions.