#!/bin/bash

# Exit when any command fails.
set -e

export CUR_PATH=`pwd`
# export ROOT_DIR="<<Insert ROOT_DIR>>"
export ROOT_DIR="/local/pmpakos/damned_directory"

# export ARMPL_CBLAS_PATH="<<Insert ARMPL_CBLAS_PATH>>"
export ARMPL_CBLAS_PATH="/local/pmpakos/arm-compiler/armpl-24.04.0_Ubuntu-22.04_gcc/"

#==========================================================================================================================================
# Install SELL-C-σ prerequisites
#==========================================================================================================================================

cd "$ROOT_DIR"
export CMAKE_DIR="$ROOT_DIR"/cmake-3.26.0-rc3/build
if [ ! -d "$CMAKE_DIR" ] 
then
	wget https://github.com/Kitware/CMake/releases/download/v3.26.0-rc3/cmake-3.26.0-rc3.tar.gz
	tar -xf cmake-3.26.0-rc3.tar.gz
	rm cmake-3.26.0-rc3.tar.gz
	cd cmake-3.26.0-rc3
	./bootstrap --prefix="$CMAKE_DIR"
	make -j
	make -j install
	cd ../
fi

wget https://download.open-mpi.org/release/hwloc/v2.10/hwloc-2.10.0.tar.gz
tar -xf hwloc-2.10.0.tar.gz
rm hwloc-2.10.0.tar.gz
cd hwloc-2.10.0
export HW_LOC_DIR="$ROOT_DIR"/hwloc-2.10.0/build

./configure --prefix="$HW_LOC_DIR"
make clean
make -j
make install
cd ../

wget http://www.fftw.org/fftw-3.3.10.tar.gz
tar -xf fftw-3.3.10.tar.gz
rm fftw-3.3.10.tar.gz
cd fftw-3.3.10
export FFTW_DIR="$ROOT_DIR"/fftw-3.3.10/build/
./configure --prefix="$FFTW_DIR"
make clean
make -j
make install
cd ../

# After building, include these two in the PATH
export PATH="$CMAKE_DIR"/bin:"$HW_LOC_DIR"/bin:$PATH

#==========================================================================================================================================
# GHOST
#==========================================================================================================================================
cd "$ROOT_DIR"
git clone https://bitbucket.org/essex/ghost.git
cd ghost
export GHOST_DIR="$ROOT_DIR"/ghost/build

# If CUDA used, need to add sm_<code> of GPU that will be used, to the CUDA_NVCC_FLAGS list in CMakeLists.txt around line 690.
# (e.g. for A100 -> list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_80,code=sm_80))

# perhaps need to add  to CMakeLists.txt
# set(GHOST_AUTOGEN_SPMMV "8,1;8,2;8,4;8,8;8,16;8,32;8,64;8,128;8,256;4,1;4,2;4,4;4,8;4,16;4,32;4,64;4,128;4,256;16,1;16,2;16,4;16,8;16,16;16,32;16,64;16,128;16,256;32,1;32,2;32,4;32,8;32,16;32,32;32,64;32,128;32,256;" CACHE STRING "SpM(M)V kernels to be auto-generated. Semicolon-separated list of 2-tuples: (CHUNKHEIGHT, NVECS)")
# but leave it for now

# f) In "ESSEX/ghost/src/sparsemat.c" -> "ghost_sparsemat_init_crs" function, change definition of function, by doing the following:
# 	i) change definition of function
# 		ghost_error ghost_sparsemat_init_crs(ghost_sparsemat *mat, ghost_gidx offs, ghost_lidx m, ghost_lidx n, ghost_lidx *col, double *val, ghost_lidx *rpt, ghost_mpi_comm mpicomm, double weight)
# 	ii) Add these lines after src.maxrowlen, so that "create_context" does not break afterwards
# 		src.gnrows = m;
# 		src.gncols = n;
sed 's/ghost_lidx n, ghost_gidx \*col, void \*val/ghost_lidx m, ghost_lidx n, ghost_lidx \*col, double \*val/; s/src.maxrowlen = n;/src.maxrowlen = n;\n\tsrc.gnrows = m; \/\/ needed to add these two, so that create_context did not fail afterwards\n\tsrc.gncols = n;/' src/sparsemat.c > src/sparsemat2.c
mv src/sparsemat2.c src/sparsemat.c

# g) In "ESSEX/ghost/include/ghost/sparsemat.h" -> ghost_sparsemat_init_crs, change definition of function, as in sparsemat.c
# 	i) add number of columns argument to function description
# 	ii) change definition of function
# 		ghost_error ghost_sparsemat_init_crs(ghost_sparsemat *mat, ghost_gidx offs, ghost_lidx m, ghost_lidx n, ghost_lidx *col, double *val, ghost_lidx *rpt, ghost_mpi_comm mpicomm, double weight);
# 	iv) In "ghost_sparsemat_rowfunc_crs" function, change "ghost_gidx *col" to "ghost_lidx *col"
# 	v) In "ghost_sparsemat_rowfunc_crs_arg" typedef struct, change "ghost_gidx *col" to "ghost_lidx *col"
sed 's/ghost_lidx n, ghost_gidx \*col, void \*val/ghost_lidx m, ghost_lidx n, ghost_lidx \*col, double \*val/; s/ghost_gidx \*col;/ghost_lidx \*col;/; s/ghost_gidx \*crscol/ghost_lidx \*crscol/; s/n The local number of rows./m The local number of rows.\n     \* @param n The local number of columns./' include/ghost/sparsemat.h > include/ghost/sparsemat2.h
mv include/ghost/sparsemat2.h include/ghost/sparsemat.h

# before running "cmake" (for ARM only, x86_64 no problem as it seems)
# 1) place "-D__FUJITSU" flag to "CMAKE_C_FLAGS" and "CMAKE_CXX_FLAGS"
# 2) place "-noansi" in comments
sed 's/-noansi//; s/${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}/${CMAKE_C_FLAGS} -D__FUJITSU ${OpenMP_C_FLAGS}/; s/${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}/${CMAKE_CXX_FLAGS} -D__FUJITSU ${OpenMP_CXX_FLAGS}/' CMakeLists.txt > CMakeLists2.txt
mv CMakeLists2.txt CMakeLists.txt

# 3) remove "include immintrin.h" (for x86 platforms only) from "src/bench.c" (if __FUJITSU correctly set in previous step, no need to do it)

# 4) download "cpuid.h" and place it in "src" directory (simply google it), place in comments first lines of "__asm__" code
# (that is useless after all, don't do it)
# wget https://raw.githubusercontent.com/gcc-mirror/gcc/master/gcc/config/i386/cpuid.h
# mv cpuid.h include/ghost

# 5) in "src/machine.c" delete all code of "ghost_machine_alignment" function calling "cpuid" function (leave only flags set to False and alignment variable set)
# sed 's/#include <cpuid.h>/#include \"cpuid.h\"/; 341,393d' src/machine.c > src/machine2.c
# sed 's/#include <cpuid.h>//; 341,393d' src/machine.c > src/machine2.c
sed 's/#include <cpuid.h>//; 346,398d' src/machine.c > src/machine2.c
mv src/machine2.c src/machine.c

# 6) in "src/gemm.c" file, place all cblas calls in comments (where "BLAS_CALL_GOTO" function calls occur)
sed '385,406d' src/gemm.c > src/gemm2.c
mv src/gemm2.c src/gemm.c

# In "ghost" root directory, create two folders, build and objdir
mkdir -p build
mkdir -p objdir

# Change directory to objdir
cd objdir

cmake .. -DCMAKE_INSTALL_PREFIX="$GHOST_DIR" -DHWLOC_INCLUDE_DIR="$HW_LOC_DIR"/include -DGHOST_USE_MPI=0 -DGHOST_USE_CUDA=0 -DCBLAS_INCLUDE_DIR="$ARMPL_CBLAS_PATH"/include
#(optional : )
#(if GPU used, add -DGHOST_USE_CUDA=1)

make clean
make -j
make install

#==========================================================================================================================================
# PHYSICS
#==========================================================================================================================================
cd "$ROOT_DIR"
git clone https://bitbucket.org/essex/physics.git
cd physics
export PHYSICS_DIR="$ROOT_DIR"/physics/build

# Remove Fortran support from CMakeLists.txt
sed 's/project (ESSEX-Physics C CXX Fortran)/project (ESSEX-Physics C CXX)/' CMakeLists.txt > CMakeLists2.txt
mv CMakeLists2.txt CMakeLists.txt

# Fix following problems too, to compile without errors
# 1) In "cheb_toolbox/color_comm_handle.c", move "MPI_Status status" that is defined outside "#ifdef MPI region" inside
# sed -n '/#ifdef GHOST_HAVE_MPI/ {h; n; s/^[ \t]*MPI_Status mpi_status;$/&/; t ok; x; p; x; p; b skip; :ok p; x; p; b skip}; p; :skip' color_comm_handle.c > sed_color_comm_handle.c # abinis magic
sed '123d; 124s/.*/\t#ifdef GHOST_HAVE_MPI\n\tMPI_Status mpi_status;/' cheb_toolbox/color_comm_handle.c > cheb_toolbox/color_comm_handle2.c
mv cheb_toolbox/color_comm_handle2.c cheb_toolbox/color_comm_handle.c

# This problem appeared in LUMI supercomputer for the first time... Need to remove the reference to this Fortran function, because we got an "undefined reference to..." when compiling ghost-apps
sed '/bessel_jn_array_/s/^/\/\/ /' cheb_toolbox/time_propagation_coeffs.cpp > cheb_toolbox/time_propagation_coeffs2.cpp
mv cheb_toolbox/time_propagation_coeffs2.cpp cheb_toolbox/time_propagation_coeffs.cpp

# 2) In "matfuncs/matfuncs_png.c" (a visualization function) remove all code refering to "png_*" functions, as it is not installed, and not needed for SELL-C-s
sed '42,67d' matfuncs/matfuncs_png.c > matfuncs/matfuncs_png2.c
mv matfuncs/matfuncs_png2.c matfuncs/matfuncs_png.c

# 3) In "matfuncs/SpinChainSZ.c", remove all references to "SMALLSPINMATRIX" (some #defines and one line of code too - It's sth in Fortran that we have disabled)
sed '339,345d; 375,377d' matfuncs/SpinChainSZ.c > matfuncs/SpinChainSZ2.c
mv matfuncs/SpinChainSZ2.c matfuncs/SpinChainSZ.c

# In "physics" root directory, create two folders, build and objdir
mkdir -p build
mkdir -p objdir

# Change directory to objdir
cd objdir

cmake .. -DCMAKE_INSTALL_PREFIX="$PHYSICS_DIR" -DGHOST_DIR="$GHOST_DIR"/lib/ghost -DFFTW3_INCLUDE_DIR="$FFTW_DIR"/include -DFFTW3_LIBRARY="$FFTW_DIR"/lib

make clean
make -j
make install

#==========================================================================================================================================
# GHOST-APPS
#==========================================================================================================================================
cd "$ROOT_DIR"
git clone https://bitbucket.org/essex/ghost-apps.git
cd ghost-apps
export GHOST_APPS_DIR="$ROOT_DIR"/ghost-apps/build

# before running "make"
# 	1) Remove Fortran support from CMakeLists.txt
# 	3) In CMakeLists, disable "cg, lanczos, cheb_dos" from being built
sed 's/project (GHOST-apps C Fortran CXX)/project (GHOST-apps C CXX)/; /lanczos/d; /cg/d; /minimal/d; /cheb_dos/d;' CMakeLists.txt > CMakeLists2.txt
mv CMakeLists2.txt CMakeLists.txt

# 	3) In "common/essexamples.c" and header file, remove function "essexamples_create_matrix_ft" (it contains an MPI_Comm in function arguments)
sed '655,970d' common/essexamples.c > common/essexamples2.c
mv common/essexamples2.c common/essexamples.c

sed '/essexamples_create_matrix_ft/d' common/essexamples.h > common/essexamples2.h
mv common/essexamples2.h common/essexamples.h

mkdir -p build
cd build

cmake .. -DCMAKE_INSTALL_PREFIX="$GHOST_APPS_DIR" -DGHOST_DIR="$GHOST_DIR"/lib/ghost -DESSEX-PHYSICS_DIR="$PHYSICS_DIR"/lib/essex-physics/
make clean
make -j20

#==========================================================================================================================================
# Finale
#==========================================================================================================================================
# now go back to benchmark for CPUs path, and build sell-C-s
cd "$CUR_PATH"/spmv_code_bench/sell-C-s/
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX="$CUR_PATH"/spmv_code_bench/sell-C-s/build -DGHOST_DIR="$GHOST_DIR"/lib/ghost -DESSEX-PHYSICS_DIR="$PHYSICS_DIR"/lib/essex-physics/

# replace placeholders with correct paths and compile sell-C-s
sed  "s|<<ROOT_DIR>>|${ROOT_DIR}|g;" ../link.txt > ./link.txt
mv link.txt ./spmvbench/CMakeFiles/spmvbench.dir

sed  "s|<<CMAKE_DIR>>/bin/cmake|${CMAKE_DIR}/bin/cmake|g" ../build.make > ./build.make
mv build.make ./spmvbench/CMakeFiles/spmvbench.dir

make -j
cd ../../../
