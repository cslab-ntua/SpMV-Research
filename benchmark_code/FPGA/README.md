Benchmark code for FPGA
=========

### Directory structure

#### csr_to_vitis_converter
This directory contains Python scripts for conversion of real matrices (stored in .mtx files) or artificial matrices (given their respective generation arguments) to Vitis Sparse Library format.

#### result_parsing
This directory contains logs of matrix generation, as well as 'dirty' logs of benchmark runs on FPGA. It also contains a Jupyter notebook used to 'clean' these logs. Nothing important here.

#### vitis_library_code
This directory contains code to run double-precision [Vitis Sparse Library](https://xilinx.github.io/Vitis_Libraries/sparse/2021.2/user_guide/L2_spmv_double_intro.html) on a Xilinx Alveo U280 FPGA. Modified version of [Vitis Accelerated Libraries](https://github.com/Xilinx/Vitis_Libraries/). Contains bash scripts to build and run the project. Detailed instructions to generate matrices and compile/run the project are provided inside the directory.
