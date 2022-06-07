SpMV-Research
=========

### Directory structure

#### artificial-matrix-generator
The project that generates artificial matrices, given specific structural features (parameters), for the SpMV benchmarks.

#### benchmark_code
Contains code to run SpMV benchmarks on each platform (CPU, GPU, FPGA) separately. For each platform, README files are provided to reproduce the results.

#### benchmark_results
Contains results of SpMV benchmark runs on each platform.

#### matrix_generation_parameters
Contains collection of artificial matrix parameters, that are used to create the dataset for the performance analysis of SpMV. The main focus should be on the [synthetic_matrices_small_dataset_simple.txt](https://github.com/p-anastas/SpMV-Research/blob/main/matrix_generation_parameters/synthetic_matrices_small_dataset_simple.txt).

#### results_visualization
Contains Jupyter notebooks that parse, combine and produce plots of the benchmark results, separately for validation and artificial matrices.

#### validation_matrices
Contains the real matrices that were used for validation of proposed methodology. The [bash script](https://github.com/p-anastas/SpMV-Research/blob/main/validation_matrices/download_validation_matrices.sh) in this directory downloads all needed matrices in .mtx format. Before running any benchmark on the real matrices, they have to be downloaded.
