Multiprocessing requires python3.7+. For silver1, python3.9 is in directory /various/pmpakos/python-3.9.7/
To use it, simply append it to your env var PATH : export PATH=/various/pmpakos/python-3.9.7/bin/:$PATH

With Python multiprocessing (40 processes) speedup of 13x is achieved. col_ind is time consuming, therefore I parallelized only this (row_ptr requires much less time to be generated).

On different machines, same number of processes and same seed gives different results! To overcome this, when col_ind for row "i" is generated, I reseed Random Number Generator with seed = seed+i. This way, whatever number of processes is used, RNG is initialized with the same given number, returning reproducible results. Overhead of RNG initialization is negligible.

Makefile produces 2 version, one for 32-bit and one for 64-bit floating point precision. CSR Represenation of the artificial matrix is returned to main (main_float or main_double respectively). Flag "VALUE_TYPE_AX" is used to determine which version will be created.

Directories : 
./include/ 
In this directory the function that is called by main, as well as the Python script that does the heavy work.

./main.c
Here you will find an example of artificial_matrix_generation function call.

matrix = artificial_matrix_generation(argc, argv, start_of_matrix_generation_args, verbose);
Command line arguments given to main are used as input to artificial_matrix_generation function.
In addition, the first position that the parameters appear in argv[] has to be given (in case your benchmark runs contain other command line arguments, adjust accordingly). 
Finally, verbose argument activates some explanatory messages regarding the progress of the generator. When running benchmarks, you can set it ot 0 (I used it as verification that matrix created in Python is transferred correctly in C).

Struct that is returned to main (csr_matrix) is defined in header file of include/ directory. 

./matrix_generation_parameters/
In this directory, separate files are provided for each :
1)precision format (32- or 64-bit floating point precision), 
2)distribution function, 
3)memory footprint (CSR - row_ptr + col_ind + values) range
containing the parameters with which we will run benchmarks.

In addition, a "small" directory is created, where 10% of the aforementioned matrix parameters is stored, for initial verification that the result collection and visualization framework is working properly.



*** *** ***
P.S. : Please, follow this results reporting format : 

foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.diagonal_factor << "," << op.seed << "," 
      << op.m << "," << op.n << "," << op.nz << "," << op.density << "," << op.mem_footprint << "," << op.mem_range << ","
      << op.avg_nz_row << "," << op.std_nz_row << "," 
      << op.avg_bandwidth << "," << op.std_bandwidth << "," << op.avg_scattering << "," << op.std_scattering << "," 
      << "cuSPARSE_coo11" << "," << op.timer << "," << gflops_s << "," << W_avg << "," << J_estimated << endl; 
*** *** ***