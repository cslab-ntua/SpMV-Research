SpMV-Research
=========

### Directory structure

#### artificial-matrix-generator
The project that generates artificial matrices, given specific structural features (parameters), for the SpMV benchmarks. The features that have to be provided to the generator are the following : 

* number of rows
* number of columns
* average nonzeros per row
* standard deviation of nonzeros per row
* distribution of nonzeros per row (currently "normal" and "gamma" distributions are supported)
* seed for the random number generator (need to reproduce results on the same artificial matrices across different platforms)
* placement of nonzeros within row (currently "random", "diagonal" and "simple" are supported)
* bandwidth of matrix, which confines the column range that a nonzero can be placed at
* coefficient of skewness, which indicates the imbalance of row length
* average number of neighboring nonzeros within row (neighbors : maximum distance 1 for column values)
* similarity between adjacent rows (based on column position of nonzeros)

#### benchmark_code
Contains code to run SpMV benchmarks on each platform (CPU, GPU, FPGA) separately. In each directory, either through a `config.sh` file or with command-line arguments, the required library and compiler paths are set so that the benchmarks can be reproduced, provided that the tested platform is available. Each subdirectory contains README files with detailed instructions on what has to be configured per platform.

#### benchmark_results
Contains results (in csv format) of SpMV benchmark runs.

#### lib
Contains several utility functions used in the benchmarks.

#### matrix_generation_parameters
Contains collection of artificial matrix parameters, that are used to create the dataset for the performance analysis of SpMV. The main focus should be on the [synthetic_matrices_medium_dataset.txt](./matrix_generation_parameters/synthetic_matrices_medium_dataset.txt). Python scripts are provided to generate the artificial matrix parameters files. Files titled "twins" contain artificial matrices that have similar structure to real matrices and are used for validation of the proposed methodology.

#### results_visualization
Contains Jupyter notebooks that parse, combine and produce plots of the benchmark results, separately for validation and artificial matrices. All paper plots were produced with these notebooks, with results from the `benchmark_results` directory.

#### validation_matrices
Contains the real matrices that were used for validation of the proposed methodology. The [bash script](./validation_matrices/get_validation_matrices.sh) in this directory downloads all required matrices in .mtx format. After downloading them, they are sorted (by row and column value) by a simple C program, as some SpMV benchmarks could run on sorted matrices only.
