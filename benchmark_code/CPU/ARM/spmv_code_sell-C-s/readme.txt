first of all, include cmake and hwloc in path (cmake is already installed in ARM machine...no need to include it)
	export PATH=/home/spmv/ESSEX/hwloc-1.11.13/build/bin:$PATH


Had to compile cmake and hwloc for compilation of "ghost" repo
	cmake version : whatever
	hwloc version : up to (and not) 2.1 (after that, intel-mic.h does not exist - drop support for intel xeon phi)



---
ghost : cmake in "build" directory with command
	cmake .. -DCMAKE_INSTALL_PREFIX=/home/spmv/ESSEX/ghost/build -DHWLOC_INCLUDE_DIR=/home/spmv/ESSEX/hwloc-1.11.13/build/include -DGHOST_USE_MPI=0

	make -j80; make install


	before running "make" (for ARM only, x86_64 no problem as it seems)
		1) place "-D__FUJITSU" flag to "CMAKE_C_FLAGS" and "CMAKE_CXX_FLAGS"
		2) place "-noansi" in comments

	during "make", fix following problems (for ARM only, x86_64 no problem as it seems)
		1) remove "include immintrin.h" (for x86 platforms only) from "src/bench.c" (if __FUJITSU correctly set in previous step, no need to do it)
		2) download "cpuid.h" and place it in "src" directory (simply google it), place in comments first lines of "__asm__" code
		3) in "src/machine.c" place all code of "ghost_machine_alignment" function calling "cpuid" function in comments (leave only flags set to False and alignment variable set)
		4) in "src/gemm.c" file, place all cblas calls in comments (where "BLAS_CALL_GOTO" function calls occur)



---
physics : 
	cmake .. -DCMAKE_INSTALL_PREFIX=/home/spmv/ESSEX/physics/build -DGHOST_DIR=/home/spmv/ESSEX/ghost/build/lib/ghost/  -DFFTW3_INCLUDE_DIR=/home/spmv/ESSEX/fftw-3.3.10/build/include/ -DFFTW3_LIBRARY=/home/spmv/ESSEX/fftw-3.3.10/build/lib
	make -j80; make install

	before running "make"
		1) Remove Fortran support from CMakeLists.txt
		2) Needed to build FFTW3 library from source
			wget http://www.fftw.org/fftw-3.3.10.tar.gz
			tar xf fftw-3.3.10.tar.gz
			cd fftw-3.3.10; mkdir build
			./configure --prefix=/home/spmv/ESSEX/fftw-3.3.10/build/
			make -j8; make install
	during "make", fix following problems
		1) In "cheb_toolbox/color_comm_handle.c", move "MPI_Status status" that is defined outside "#ifdef MPI region" inside
		2) In "matfuncs/matfuncs_png.c" (a visualization function) place in comments all code refering to "png_*" functions, as it is not installed, and not needed for SELL-C-s
		3) In "matfuncs/SpinChainSZ.c", place in comments all references to "SMALLSPINMATRIX" (some #defines and one line of code too - It's sth in Fortran that we have disabled)



---
ghost-apps : 
	cmake .. -DCMAKE_INSTALL_PREFIX=/home/spmv/ESSEX/ghost-apps/build -DGHOST_DIR=/home/spmv/ESSEX/ghost/build/lib/ghost/ -DESSEX-PHYSICS_DIR=/home/spmv/ESSEX/physics/build/lib/essex-physics/
	make -j20

	before running "make"
		1) Remove Fortran support from CMakeLists.txt
		3) In CMakeLists, disable "cg, lanczos, cheb_dos" from being built
		3) In "common/essexamples.c" and header file, place in comments function "essexamples_create_matrix_ft" (it contains an MPI_Comm in function arguments)



---
To run spmvbench

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
 -c, --cores=CORES		Number of cores to be used
 -d, --densemat_storage=ORDER	Densemat storage order, valid values:
				  (1) row (row-major, default for densemats with ncols>1)
				  (2) col (column-major, default for densemats with ncols=1)
 -b, --num columns		Number of columns in a block vector
 -g, --gpu=GPU			GPU to be used for CUDA runs
 -f, --matformat=FORMAT		Sparse matrix storage format
 -i, --iterations=NITER		Number of iterations for benchmarking (default: 100)
 -m, --matrix=MATRIX		Sparse matrix to be used, valid values:
				  (1) Any Matrix Market file (must end with .mm or .mtx)
				  (2) Any binary CRS file
				  (3) Spin-<NUp>-<disorder>-<seed>
				  (4) Topi-<Nx>-<Ny>-<Nz>
				  (5) Graphene-<Nx>-<Ny>
				  (5) HPCG-<Nx>-<Ny>-<NZ>
 -s, --spmvmode=MODE		Sparse matrix vector solver, valid values:
				  "Vector" (blocking communication, followed by computation) (default)
				  "Overlap" (implicitly overlap communication and computation with non-blocking MPI)
				  "Task" (explicitly overlap communication and computation with GHOST tasks)
				  "Nocomm" (no communication of vector data)
				  "Pipelined" (pipelined block vector communication, EXPERIMENTAL)
 -r, --eig_range=<lo>,<hi>	Estimation of extremal eigenvalues  for Chebyshev or Feast Methods
 -R, --random_vectors=NVECS	Number of random vectors used in Density Chebyshev Methods
 --RCM				Apply RCM permutation to the sparse matrix
 -M, --chebyshev_moments=NMOMENTS	Number of Chebyshev moments used in Chebyshev Methods, or subspace dimension in FEAST method
 -C, --chebyshev_mode=MODESTRING	Chebyshev method config string: {s,m}-{n,o}-{s,c}
				  s,m = single/block vector
				  n,o = naive/optimized kernel (optimized kernel uses augmented SpM(M)V
				  s,c = reduce dot products in each iteration/once at the end
 -o, --output_file=FILENAME	Output file for computed data
 -t, --threadspercore=THREADS	Number of threads per core
 -v, --verbose
 -w, --weights=CPU:GPU:MIC	Weight of CPU, GPU and MIC processes
 -x, --cp_freq=FREQUENCY	Checkpointing frequency
 --Zoltan				Apply Zoltan partitioning for communication minimization

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

example run : 
	./spmvbench -m <MATRIX> -c <NUM_THREADS> -b 1 -s=Vector -t 1 -i 128 -f SELL-4-1


---
To build with artificial matrix generator
1) Changes in 
	a) spmvbench.c : Replace it with what is in repo
	b) common/essexamples.c : Replace it with what is in repo
	c) common/essexamples.h : declaration of function "essexamples_create_artificial_matrix"
	d) build/spmvbench/CMakeFiles/spmvbench.dir/build.make : 
		i) After "include spmvbench/CMakeFiles/spmvbench.dir/flags.make", add : 

				##################################################################################################################
				AMG_PATH = /various/pmpakos/SpMV-Research/artificial-matrix-generator
				LIB_PATH = /various/pmpakos/SpMV-Research/lib
				AMG_CFLAGS=-D'DOUBLE=1'  -I'$(AMG_PATH)' -I'${LIB_PATH}' -g -O3 -pedantic -fopenmp -Wall -Wno-unused-local-typedefs
				##################################################################################################################

		ii) In "spmvbench/CMakeFiles/spmvbench.dir/spmvbench.c.o" rule, add "-I${AMG_PATH} -I${LIB_PATH}", so that header will be included
		iii) Add following rules, for object files of artificial-matrix-generator to be built
				##################################################################################################################
				CMakeFiles/spmvbench.dir/artificial_matrix_generation.o: ${AMG_PATH}/artificial_matrix_generation.c
					cd /home/spmv/ESSEX/ghost-apps/build/spmvbench && /usr/bin/cc -o $@ -c $< ${AMG_CFLAGS}

				CMakeFiles/spmvbench.dir/ordered_set.o: ${AMG_PATH}/ordered_set.c
					cd /home/spmv/ESSEX/ghost-apps/build/spmvbench && /usr/bin/cc -o $@ -c $< ${AMG_CFLAGS}

				CMakeFiles/spmvbench.dir/rapl.o: $(LIB_PATH)/monitoring/power/rapl.c
					cd /home/spmv/ESSEX/ghost-apps/build/spmvbench && /usr/bin/cc -o $@ -c $< ${AMG_CFLAGS}
				##################################################################################################################
	e) build/spmvbench/CMakeFiles/spmvbench.dir/link.txt : add "CMakeFiles/spmvbench.dir/artificial_matrix_generation.o CMakeFiles/spmvbench.dir/ordered_set.o CMakeFiles/spmvbench.dir/rapl.o" after "spmvbench.c.o"


	f) In "ESSEX/ghost/src/sparsemat.c" -> "ghost_sparsemat_init_crs" function, change definition of function, by doing the following:
		i) change definition of function
			ghost_error ghost_sparsemat_init_crs(ghost_sparsemat *mat, ghost_gidx offs, ghost_lidx m, ghost_lidx n, ghost_lidx *col, double *val, ghost_lidx *rpt, ghost_mpi_comm mpicomm, double weight)
		ii) Add these lines after src.maxrowlen, so that "create_context" does not break afterwards
			src.gnrows = m;
			src.gncols = n;
	g) In "ESSEX/ghost/src/sparsemat.h" -> ghost_sparsemat_init_crs, change definition of function, as in sparsemat.c
		i) add number of columns argument to function description
		ii) change definition of function
			ghost_error ghost_sparsemat_init_crs(ghost_sparsemat *mat, ghost_gidx offs, ghost_lidx m, ghost_lidx n, ghost_lidx *col, double *val, ghost_lidx *rpt, ghost_mpi_comm mpicomm, double weight);
		iv) In "ghost_sparsemat_rowfunc_crs" function, change "ghost_gidx *col" to "ghost_lidx *col"


---
Example runs
	artificial matrix
		export OMP_NUM_THREADS=80; ./spmvbench -c $OMP_NUM_THREADS -f SELL-16-4 --artif_args="655350 655350 5 1.6667 normal random 0.05 0 0.05 0.05 14"

	validation matrix
		export OMP_NUM_THREADS=80; ./spmvbench -c $OMP_NUM_THREADS -f SELL-16-4 -m /various/pmpakos/SpMV-Research/validation_matrices/raefsky3.mtx
