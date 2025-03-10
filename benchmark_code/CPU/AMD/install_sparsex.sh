#!/bin/bash

# Exit when any command fails.
set -e

# Change the install root directory for the libraries to what you prefer.
export ROOT_DIR="<<Insert ROOT_DIR>>"
# export ROOT_DIR=/various/pmpakos/icy3_libs
# export ROOT_DIR=/various/pmpakos/epyc5_libs


export BOOST_ROOT_DIR="${ROOT_DIR}/boost_1_55_0"
export LLVM_ROOT_DIR="${ROOT_DIR}/llvm-6.0.0"
export SPARSEX_ROOT_DIR="${ROOT_DIR}/sparsex"

# export BOOST_LIB_PATH="${BOOST_ROOT_DIR}/bin/lib"
# export LLVM_LIB_PATH="${LLVM_ROOT_DIR}/build/lib"

#==========================================================================================================================================
# Install cmake (if not installed already)
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
	make -j8
	make -j8 install
	cd ../
fi

# After building, include these two in the PATH
export PATH="$CMAKE_DIR"/bin:$PATH

#==========================================================================================================================================
# BOOST
#==========================================================================================================================================

cd "$ROOT_DIR"

# First, need to build lib-boost
# Download boost library 1.55 from https://sourceforge.net/projects/boost/files/boost/1.55.0/
wget https://netix.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz
tar -xf boost_1_55_0.tar.gz
rm boost_1_55_0.tar.gz

cd ${BOOST_ROOT_DIR}

# Then run to create the "b2" executable (only specified libraries are needed - found in configure of sparsex library)
./bootstrap.sh --with-libraries=atomic,regex,serialization,system,thread --prefix=${BOOST_ROOT_DIR}/bin

# Then, run "b2" with install option too
# This will install it under --prefix that was specified in "bootstrap.sh"
./b2 install


#==========================================================================================================================================
# LLVM
#==========================================================================================================================================

cd "$ROOT_DIR"

# Then, llvm of version 4.0.0 up to 6.0.0 must be used
# Download llvm-6 and clang source directories from https://releases.llvm.org/6.0.0/
wget https://releases.llvm.org/6.0.0/llvm-6.0.0.src.tar.xz
tar -xf llvm-6.0.0.src.tar.xz
mv llvm-6.0.0.src llvm-6.0.0
rm llvm-6.0.0.src.tar.xz

# 'cfe-6.0.0.src' needs to be but in llvm-6.0.0/tools and renamed to 'clang'.
wget https://releases.llvm.org/6.0.0/cfe-6.0.0.src.tar.xz
tar -xf cfe-6.0.0.src.tar.xz
mv cfe-6.0.0.src llvm-6.0.0/tools/clang
rm cfe-6.0.0.src.tar.xz

cd "$LLVM_ROOT_DIR"

# In llvm-6.0.0 root directory, create two folders, build and objdir
mkdir -p build
mkdir -p objdir

# Change directory to objdir
cd objdir

# Run 
time cmake .. -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="${LLVM_ROOT_DIR}/build" -DCMAKE_BUILD_TYPE=Release "${LLVM_ROOT_DIR}"
time make -j8
time make -j8 install


#==========================================================================================================================================
# SparseX
#==========================================================================================================================================

cd "$ROOT_DIR"

# Then, build SparseX
git clone 'https://github.com/cslab-ntua/sparsex.git'

cd "$SPARSEX_ROOT_DIR"

autoreconf -vi

# NOT NEEDED:
#     In "configure" file, replace following to find the installed boost version (--with-boostlibdir doesn't work for some reason)
#        #include "$d/include/boost/version.hpp"      ->       #include "${SPARSEX_ROOT_DIR}/boost_1_55_0/bin/include/boost/version.hpp"
#        BOOST_CPPFLAGS="-I$d/include"                ->       BOOST_CPPFLAGS="-I${SPARSEX_ROOT_DIR}/boost_1_55_0/bin/include/"

# 'configure' uses 'sh' and it fails at 'test 6.0.0 == 6.0.0' at line 17535 because it doesn't support the '==' operator...
# Just use bash like a normal person.
sed 's/#!\s*\/bin\/sh/#!\/bin\/bash/' configure > configure.new
rm configure
mv configure.new configure
chmod +x configure

# At long last, configure.
./configure --disable-silent-rules  --prefix="${SPARSEX_ROOT_DIR}/build" --with-value=double --with-boostdir="${BOOST_ROOT_DIR}/bin" --with-boostlibdir="${BOOST_ROOT_DIR}/bin/lib" --with-llvm=${LLVM_ROOT_DIR}/build/bin/llvm-config

# Remove "examples" subdirs in variables "SUBDIRS" kai "DIST_SUBDIRS" in src/Makefile (not first level Makefile in sparsex directory)
sed 's/examples//' src/Makefile > src/Makefile.new
rm src/Makefile
mv src/Makefile.new src/Makefile

# Is this needed?
#     Also, change everywhere
#        "AM_DEFAULT_VERBOSITY = 0"     ->     "AM_DEFAULT_VERBOSITY = 1"

make -j8
make -j8 install
make check

# After building successfully, ready to test it (test_sparsex.c).
# Simply run "make" in this directory, after setting the SPARSEX_ROOT_DIR, BOOST_LIB_PATH, LLVM_LIB_PATH environment variables in config.sh accordingly.
# export SPARSEX_ROOT_DIR=/various/dgal/epyc1/sparsex
# Validation matrix example:
# 	export t=24; export OMP_NUM_THREADS=$t; export mt_conf=$(seq -s ',' 0 1 "$(($t-1))"); ./spmv_sparsex.exe scircuit.mtx -t -o spx.rt.nr_threads=$t spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all -v
# Artificial matrix example:
# 	export t=24; export OMP_NUM_THREADS=$t; export mt_conf=$(seq -s ',' 0 1 "$(($t-1))"); ./spmv_sparsex.exe -p "65535 65535 5 1.6667 normal random 0.05 0 0.05 0.05 14" -t -o spx.rt.nr_threads=$t spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all -v

