SpMV targeting ARM CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* Altra Q80-33 processor (Ampere 'Mount Jade' server)

**Software :**

* [ARM C/C++ Compiler](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/cpp-compiler) 22.0.1
* [Arm Performance Libraries](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/performance-libraries) 22.0.1

### How to compile

For the SpMV benchmarks on this ARM CPU the armclang compiler was used, to obtain optimal performance. You first have to download the ARM Compiler (downloading from this [link](https://developer.arm.com/downloads/-/arm-compiler-for-linux) Performance Libraries are included). After untar-ing, install the compilers in the selected path (name it `<ARM-Compiler-Path>`).

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate ARM compiler path and benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...) and then you run `make` in the `spmv_code` directory.

### How to run

To run SpMV you simply run the run.sh bash script file. All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
