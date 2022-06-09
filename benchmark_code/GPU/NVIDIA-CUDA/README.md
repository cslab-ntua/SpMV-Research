SpMV targeting NVIDIA GPUs
=========

### Hardware/Software Requirements
**Hardware :**

* NVIDIA P100 GPU
* NVIDIA V100 GPU
* NVIDIA A100 GPU

**Software :**

* CMAKE VERSION >= 3.10
* CUDA toolkit versions 9.x and 11.x
* [Energy-measurment-tool (submodule -> working commit e480f66 on Oct 18, 2021)](https://github.com/p-anastas/nvidia-energy-measure)

### Dependency Notes

* The spmv_code_csr5_cuda *integrates* the original CSR5 (cuda version) code (working commit 4b06ce3 on Dec 26, 2020) (https://github.com/weifengliu-ssslab/Benchmark_SpMV_using_CSR5/). 
* Compiling spmv_code_csr5_cuda requires cuda 9.x since original code was not updated for cuda 11.x.
* NVIDIA A100 compilation requires CUDA toolkit version >= 11.x. Benchmarks which use CUDA 9.x (spmv_code_cusparse-9.x and spmv_code_csr5_cuda) are not supported.

### How to compile

To compile this benchmark, you need to first edit the `config.sh` file with the appropriate library paths and benchmark configurations that you want (number of threads, desired matrices, spmv kernels ...) and then you run `make` in both the `spmv_code_csr5` and `spmv_code_mkl-naive` directories.

### How to run

To run the SpMV benchmarks you simply run the `run.sh` bash script file.
All the configuration needed is in the `config.sh` file, along with documentation comments for each option.
