SPARSEX_ROOT_DIR=/home/pmpakos/sparsex


---


First, need to build lib-boost
Download boost library 1.55 from https://sourceforge.net/projects/boost/files/boost/1.55.0/
	wget https://netix.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz

Then run to create the "b2" executable (only specified libraries are needed - found in configure of sparsex library)
	./bootstrap.sh --with-libraries=atomic,regex,serialization,system,thread --prefix=${SPARSEX_ROOT_DIR}/boost_1_55_0/local

Then, run "b2" with install option too
	./b2 install

This will install it under --prefix that was specified in "bootstrap.sh"


---


Then, llvm of version 4.0.0 up to 6.0.0 must be used
Download llvm-6 and clang source directories from https://releases.llvm.org/6.0.0/
	wget https://releases.llvm.org/6.0.0/llvm-6.0.0.src.tar.xz
	wget https://releases.llvm.org/6.0.0/cfe-6.0.0.src.tar.xz

Then, to build, run the following
Untar llvm.tar.xz (tar xf llvm-6.0.0.src.tar.xz)
Rename llvm-6.0.0.src to llvm-6.0.0
Untar cfe.tar.xz (tar xf cfe-6.0.0.src.tar.xz) in directory llvm-6.0.0/tools/
Rename cfe-6.0.0.src to clang
In llvm-6.0.0 root directory, create two folders, build and objdir
Change directory to objdir

Run 
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=${SPARSEX_ROOT_DIR}/llvm-6.0.0/build -DCMAKE_BUILD_TYPE=Release ${SPARSEX_ROOT_DIR}/llvm-6.0.0/\
    make -j20
    make install


---


Then, build SparseX


Run "autoreconf -vi" and then "./configure"

In "configure" file, replace following to find the installed boost version (--with-boostlibdir doesn't work for some reason)

#include "$d/include/boost/version.hpp"      ->       #include "${SPARSEX_ROOT_DIR}/boost_1_55_0/local/include/boost/version.hpp"
BOOST_CPPFLAGS="-I$d/include"                ->       BOOST_CPPFLAGS="-I${SPARSEX_ROOT_DIR}/boost_1_55_0/local/include/"


Then run
	./configure --disable-silent-rules  --prefix=${SPARSEX_ROOT_DIR}/build --with-value=double --with-boostlibdir=${SPARSEX_ROOT_DIR}/boost_1_55_0/local/lib/ --with-llvm=${SPARSEX_ROOT_DIR}/llvm-6.0.0/build/bin/llvm-config


Remove "examples" subdirs in variables "SUBDIRS" kai "DIST_SUBDIRS" in src/Makefile (not first level Makefile in sparsex directory)
Also, change everywhere
    "AM_DEFAULT_VERBOSITY = 0"     ->     "AM_DEFAULT_VERBOSITY = 1"

And run
	make -j8
	make install
	make check


---


After building successfully, ready to test it (test_sparsex.c). Simply run "make" in this directory, after setting the SPARSEX_ROOT_DIR, BOOST_LIB_PATH, LLVM_LIB_PATH environment variables in config.sh accordingly


validation matrix example
	export t=80; export OMP_NUM_THREADS=$t; export mt_conf=$(seq -s ',' 0 1 "$(($t-1))"); ./spmv_sparsex.exe scircuit.mtx -t -o spx.rt.nr_threads=$t spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all -v
artificial matrix example
	export t=80; export OMP_NUM_THREADS=$t; export mt_conf=$(seq -s ',' 0 1 "$(($t-1))"); ./spmv_sparsex.exe -p "65535 65535 5 1.6667 normal random 0.05 0 0.05 0.05 14" -t -o spx.rt.nr_threads=$t spx.rt.cpu_affinity=${mt_conf} -o spx.preproc.xform=all -v
