SpMV targeting Xilinx FPGAs
=========
<!-- _(Modified version of README from Vitis Sparse Library)_ -->

### Hardware/Software Requirements
**Hardware :**

* Xilinx Alveo U280 Data Center Accelerator Card

**Software :**

* Vitis Unified Software Platform 2020.2 (or greater)
* Xilinx Runtime (XRT) 2020.2 (or greater)
* Development & Deployment Target Platforms for Alveo U280, compatible with Xilinx tools 2020.2, available [here](https://www.xilinx.com/products/boards-and-kits/alveo/u280.html#gettingStarted)
* [Vitis Accelerated Libraries](https://github.com/Xilinx/Vitis_Libraries/) 2021.1 (or greater)

### How to compile
To compile this benchmark, you first have to download the Vitis Accelerated Libraries and replace the `sparse/L2/benchmarks/spmv_double` directory with the code of this directory.

**Generate build file :**

Run the following commands to generate a build script and build your XCLBIN and host binary targeting Alveo U280. 
The arguments for the ```gen_build.sh``` script should be provided between ' ' (apostrophes). Currently the only supported Target Device is the Xilinx Alveo U280 (xilinx_u280_xdma_201920_3).

Please note that the hardware building process will take a long time. It can be 8-10 hours. 

```
    bash gen_build.sh -l <Xilinx Tools License Path> -v <Vitis Tools Path> -x <XRT Path> \
                      -p <Xilinx Platforms Path> -d <Target Device>
    bash build.sh
```

### How to run

To run SpMV with the specific library, an offline conversion must be performed for each matrix tested. Python scripts (modified versions of scripts provided by Vitis Libraries) are used for the conversion, separate for real and artificial matrices.

**Generate real-matrix inputs :**

```
    python3 ../csr_to_vitis_converter/gen_signature.py --partition --mtx_list validation_list.txt --sig_path ./sig_dat
    python3 ../csr_to_vitis_converter/gen_vectors.py --gen_vec --mtx_list validation_list.txt --vec_path ./vec_dat
```
This triggers a set of python scripts to convert real matrices stored in .mtx files, whose path is stored in the txt file, and partition it evenly across 16 HBM channels. Each partitioned data set, including the value and indices of each NNZ entry, is stored in one HBM channel. Each row of the partitioned data set is padded to multiple of 32 to accommodate the double precision accumulation latency. The padding overhead for each matrix is summarized in the benchmark result as well. This overhead will be reduced with the improvement of floating point support on FPGA platforms.

**Generate artificial-matrix inputs :**

```
    python3 ../csr_to_vitis_converter/gen_signature_synthetic.py --partition \
            --mtx_param_list ../../../matrix_generation_parameters/synthetic_matrices_small_dataset.txt \
            --sig_path ./sig_dat --vec_path ./vec_dat
```
This triggers a set of python scripts to generate every artificial matrix, whose parameters are described in the txt file. The same partitioning strategy is followed, as described for real matrices.
<!-- , and partition it evenly across 16 HBM channels. Each partitioned data set, including the value and indices of each NNZ entry, is stored in one HBM channel. Each row of the partitioned data set is padded to multiple of 32 to accommodate the double precision accumulation latency. The padding overhead for each matrix is summarized in the benchmark result as well. This overhead will be reduced with the improvement of floating point support on FPGA platforms. -->

**Run tests:**

```
    bash gen_run.sh -l <Xilinx Tools License Path> -v <Vitis Tools Path> -x <XRT Path>
    bash run.sh 
```
First, generate the run script, for all matrices that are generated in the `sig_dat` directory. This script generates a bash script that performs 5 replications of SpMV runs on each matrix in our dataset. Results from running the benchmark are stored in a separate file per replication.
