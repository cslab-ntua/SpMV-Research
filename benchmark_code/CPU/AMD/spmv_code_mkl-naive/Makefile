.phony: all clean

CPATH = 
define NEWLINE


endef

shell_out := $(shell ../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

AMG_PATH = ../../../../artificial-matrix-generator
CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c $(AMG_PATH)/ordered_set.c

CFLAGS = 
LDFLAGS =

executables = 

# executables += spmv_coo.exe
# executables += spmv_dia.exe
# executables += spmv_csc.exe
# executables += spmv_csr.exe
executables += spmv_sparse_mv.exe
# executables += spmv_bsr_2.exe spmv_bsr_4.exe spmv_bsr_8.exe spmv_bsr_16.exe spmv_bsr_32.exe spmv_bsr_64.exe
executables += spmv_csr_naive.exe
# executables += spmv_csr_custom.exe
executables += spmv_csr_custom_vector.exe
executables += spmv_csr_custom_vector_perfect_nnz_balance.exe
# executables += spmv_dia_custom.exe
# executables += spmv_ldu.exe
# executables += spmv_ell.exe

all: $(executables)


CC=g++
# CC=clang++

CFLAGS += -m64 -Wall
CFLAGS += -mavx2

CFLAGS += -I'../../../../lib'
CFLAGS += -I'$(AMG_PATH)' -I'$(AMG_PATH)/lib'

CFLAGS += -I'$(MKL_PATH)/include'

# CFLAGS += -m64 -Wall -I/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl/include -I/home/users/nikela/local/papi/include#-Wno-unused-variable #-funroll-all-loops -fopenmp
# CFLAGS += -m64 -Wall -I/usr/include/mkl -I/home/users/nikela/local/papi/include -I/opt/intel/oneapi/mkl/latest/include

# LDFLAGS += -L/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl/lib/intel64 -L/home/users/nikela/local/papi/lib -L/opt/intel/oneapi/mkl/latest/lib/intel64
# LDFLAGS += -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl -lpapi

# LDFLAGS += -L/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl/lib/intel64 -L/home/users/nikela/local/papi/lib -L/opt/intel/oneapi/mkl/latest/lib/intel64
LDFLAGS += -L'$(MKL_PATH)/lib/intel64'
LDFLAGS += -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl -lpapi

CFLAGS += -fopenmp
CFLAGS += -Wno-deprecated-declarations

CFLAGS += -march=native

# CFLAGS += -O0
CFLAGS += -O3

CFLAGS += -flto


# CFLAGS += -fopt-info-vec-missed=vect.miss
# CFLAGS += -fopt-info-loop-optimized=vect.opt
# CFLAGS += -ftree-vectorizer-verbose=1

# DOUBLE := 0
DOUBLE := 1

CFLAGS += -D'DOUBLE=$(DOUBLE)'


# PROC_BENCH = 1

ifdef PROC_BENCH
    CFLAGS += -D'PROC_BENCH' 
endif


# spmv_all: main.cpp
# $(CC) main.cpp $(CFLAGS) -o $@ $(LDFLAGS)

spmv_sparse_mv.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_IE' -o $@ $(LDFLAGS)
spmv_csr.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_CSR' -o $@ $(LDFLAGS)

spmv_csr_naive.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -o $@ $(LDFLAGS)
spmv_csr_custom.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -o $@ $(LDFLAGS)
spmv_csr_custom_vector.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'CUSTOM_VECTOR' -o $@ $(LDFLAGS)
spmv_csr_custom_vector_perfect_nnz_balance.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' -o $@ $(LDFLAGS)

spmv_dia.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_DIA' -o $@ $(LDFLAGS)
spmv_dia_custom.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_DIA' -o $@ $(LDFLAGS)

spmv_bsr_2.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=2'  -o $@ $(LDFLAGS)
spmv_bsr_4.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=4'  -o $@ $(LDFLAGS)
spmv_bsr_8.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=8'  -o $@ $(LDFLAGS)
spmv_bsr_16.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=16' -o $@ $(LDFLAGS)
spmv_bsr_32.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=32' -o $@ $(LDFLAGS)
spmv_bsr_64.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=64' -o $@ $(LDFLAGS)

spmv_csc.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_CSC' -o $@ $(LDFLAGS)

spmv_coo.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_MKL_COO' -o $@ $(LDFLAGS)

spmv_ldu.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_LDU' -o $@ $(LDFLAGS)

spmv_ell.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_ELL' -o $@ $(LDFLAGS)


clean:
	rm -f *.exe
