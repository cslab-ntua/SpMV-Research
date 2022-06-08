SpMV targeting ARM CPUs
=========

### Hardware/Software Requirements
**Hardware :**

* Altra Q80-33 processor (Ampere 'Mount Jade' server)

**Software :**

* Ubuntu 21.10
* [ARM C/C++ Compiler](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/cpp-compiler) 22.0.1
* [Arm Performance Libraries](https://www.arm.com/products/development-tools/server-and-hpc/allinea-studio/performance-libraries) 22.0.1
* Environment Modules (to load ARM compiler and libraries)

### How to compile
To compile this benchmark, you first have to download the ARM Compiler (Downloading from this [link](https://developer.arm.com/downloads/-/arm-compiler-for-linux) Performance Libraries are included). After untar-ing, install the compilers in the selected path.

```
    ./arm-compiler-for-linux_22.0.1_Ubuntu-20.04.sh -i <ARM-Compiler-Path> -a
```

To easily use the compiler you have to load modulefiles that are included in the installation of the ARM compiler. To do so, you have to set Module to search for Modulefiles in the directory `<ARM-Compiler-Path>/modulefiles`. 

```
    export MODULEPATH=$MODULEPATH:<ARM-Compiler-Path>/modulefiles
```

After setting the correct path, replace the directory with the provided modulefiles directory (needed to set Performance Libraries environment variables automatically). After running the following command, the proper environment variables have been set to compile and run programs with the ARM Compiler and Performance Libraries.

```
    module load acfl/22.0.1
```

After this, running `make` builds the two formats examined for the ARM CPU (csr-naive and ARM-library).

### How to run

To run SpMV with the specific library, an offline conversion must be performed for each matrix tested. Python scripts (modified versions of scripts provided by Vitis Libraries) are used for the conversion, separate for real and artificial matrices.

<!-- 
```
    python3 ../csr_to_vitis_converter/gen_signature_synthetic.py --partition \
            --mtx_param_list ../../../matrix_generation_parameters/synthetic_matrices_small_dataset.txt \
            --sig_path ./sig_dat --vec_path ./vec_dat
``` -->
<!-- This triggers a set of python scripts to generate every artificial matrix, whose parameters are described in the txt file. The same partitioning strategy is followed, as described for real matrices.
 -->

**Run tests:**

<!-- ```
    bash gen_run_em.sh -l <Xilinx Tools License Path> -v <Vitis Tools Path> -x <XRT Path>
    bash run_em.sh 
```
First, generate the run script, for all matrices that are generated in the `sig_dat` directory. This script generates a bash script that performs 5 replications of SpMV runs on each matrix in our dataset. Results from running the benchmark are stored in a separate file per replication.
 -->