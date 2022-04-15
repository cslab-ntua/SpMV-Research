spmv_double
=========
_(Modified version of README from Vitis)_

To run this benchmark, you first have to download the [Vitis Accelerated Libraries](https://github.com/Xilinx/Vitis_Libraries/) and replace the ``sparse/L2/benchmarks/spmv_double`` directory with the code of this directory. 

The tutorial provides a step-by-step guide that covers commands for building and running the benchmark.

Executable Usage
-----------------

* **Build hardware and host executable** 

Run the following make commands to build your XCLBIN and host binary targeting Alveo U280. Please note that the hardware building process will take a long time. It can be 8-10 hours.

```
    make all TARGET=hw PLATFORM_REPO_PATHS=/opt/xilinx/platforms DEVICE=xilinx_u280_xdma_201920_3
```

* **Run benchmark**

To get the benchmark results, please run the following command.

**Generate test inputs:**

```
    python3 ../csr_to_vitis_converter/gen_signature_synthetic.py --partition --mtx_param_list ../../../matrix_generation_parameters/synthetic_matrices_small_dataset.txt --sig_path ./sig_dat --vec_path ./vec_dat

```
This triggers a set of python scripts to generate every artificial matrix, whose parameters are described in the txt file, and partition it evenly across 16 HBM channels. Each partitioned data set, including the value and indices of each NNZ entry, is stored in one HBM channel. Each row of the partitioned data set is padded to multiple of 32 to accommodate the double precision accumulation latency. The padding overhead for each matrix is summarized in the benchmark result as well. This overhead will be reduced with the improvement of floating point support on FPGA platforms.

**Run tests:**

```
    bash gen_run_em.sh
    bash run_em.sh
    
```
First, generate the run script, for all matrices that are generated in the sig_dat directory. This script generates a bash script that performs 5 replications of SpMV runs on each matrix in our dataset. Results from running the benchmark are stored in a separate file per replication.
