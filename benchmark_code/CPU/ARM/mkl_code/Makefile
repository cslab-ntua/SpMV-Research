.phony: all clean

USERNAME := $(shell whoami)
REMOTE_MACHINE := $(findstring xexdgala,$(HOSTNAME))

C_LIB_PATH = 
# library = ${C_LIB_PATH}

ifdef REMOTE_MACHINE
    max_cores = 128
else
    max_cores = 8
endif


# PYTHON_CFLAGS = -fPIE $(shell python3-config --cflags)
# PYTHON_LFLAGS = $(shell python3-config --ldflags --embed)

# AMG_PATH = artificial_matrix_generation/include
# CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c

AMG_PATH = ../artificial-matrix-generator
CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/ordered_set.c


CFLAGS = 
LDFLAGS =

executables = 
# executables += spmv_coo.exe
# executables += spmv_dia.exe
# executables += spmv_csc.exe
# executables += spmv_csr.exe
# executables += spmv_sparse_mv.exe
# executables += spmv_bsr_2.exe spmv_bsr_4.exe spmv_bsr_8.exe spmv_bsr_16.exe spmv_bsr_32.exe spmv_bsr_64.exe
executables += spmv_csr_naive.exe
# executables += spmv_csr_custom.exe
# executables += spmv_csr_custom_vector.exe
# executables += spmv_dia_custom.exe
# executables += spmv_ldu.exe
# executables += spmv_ell.exe

all: $(executables)


CC=g++
# CC = ${HOME}/Documents/gcc_current/bin/g++


# CFLAGS += -m64 -Wall -I/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl/include -I/home/users/nikela/local/papi/include#-Wno-unused-variable #-funroll-all-loops -fopenmp -m64
CFLAGS += -Wall
# CFLAGS += -mavx2

# LDFLAGS += -L/various/common_tools/intel_parallel_studio/compilers_and_libraries/linux/mkl/lib/intel64 -L/home/users/nikela/local/papi/lib -L/opt/intel/oneapi/mkl/latest/lib/intel64
LDFLAGS += -Wl,--no-as-needed -lgomp -lpthread -lm -ldl -lpapi

CFLAGS += -fopenmp
CFLAGS += -Wno-deprecated-declarations

# CFLAGS += -O0
CFLAGS += -O3


# CFLAGS += -I "$(library)"
CFLAGS += -Ilib
CFLAGS += -I$(AMG_PATH) -I$(AMG_PATH)/lib


# DOUBLE := 0
DOUBLE := 1

# ifeq (DOUBLE, 0)
    # CFLAGS += -D'VALUE_TYPE_AX=float'
# else
    # CFLAGS += -D'VALUE_TYPE_AX=double'
# endif

CFLAGS += -D'DOUBLE=$(DOUBLE)'


# CFLAGS += -D'PARALLEL_INIT'


# CFLAGS += $(PYTHON_CFLAGS)

# LDFLAGS += $(PYTHON_LFLAGS)


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
