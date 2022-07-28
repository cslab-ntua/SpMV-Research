.PHONY: all clean

SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables

# 'shell' merges all lines into one, but gcc -M ends each line with '\' (backslash), so we have to remove them.
define Rule_Auto_Dependencies =
    $(info Searching dependencies: $(2))
    $(shell gcc $(CFLAGS) -MT '$(1)' -MM -MG '$(2)' | sed 's/\\$$//g')
endef

library = ../../../../lib


CPATH = 
define NEWLINE


endef

shell_out := $(shell ../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

AMG_PATH = ../../../../artificial-matrix-generator


CC = gcc
# CC = clang

CPP = g++
# CPP = clang++


CFLAGS = -Wall -Wextra
CFLAGS += -Wno-deprecated-declarations
# CFLAGS += -Wno-unused-variable
CFLAGS += -fopenmp

# CFLAGS += -g
# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

CFLAGS += -flto
CFLAGS += -march=native

CFLAGS += -m64
CFLAGS += -mavx2

CFLAGS += -I'$(library)'
CFLAGS += -I'$(AMG_PATH)'
CFLAGS += -I'$(MKL_PATH)/include'
CFLAGS += -I'/usr/include/mkl'
CFLAGS += -I'$(AOCL_PATH)/include' -I'$(AOCL_PATH)/../../library/src/include'

# DOUBLE := 0
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'

# PROC_BENCH = 1
ifdef PROC_BENCH
    CFLAGS += -D'PROC_BENCH' 
endif


LDFLAGS =

LDFLAGS_MKL =
LDFLAGS_MKL += -lm
LDFLAGS_MKL += -L'$(MKL_PATH)/lib/intel64'
LDFLAGS_MKL += -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

LDFLAGS_AOCL =
LDFLAGS_AOCL += -lm
LDFLAGS_AOCL += -L'$(AOCL_PATH)/lib/'
LDFLAGS_AOCL += -Wl,--no-as-needed  -laoclsparse -lgomp -lpthread -lm -ldl

LDFLAGS_AOCL =
LDFLAGS_AOCL += -lm
LDFLAGS_AOCL += -L'$(AOCL_PATH)/lib/'
LDFLAGS_AOCL += -Wl,--no-as-needed  -laoclsparse -lgomp -lpthread -lm -ldl #-lpapi

LIB_SRC = read_mtx.cpp csr_converter.c artificial_matrix_generation.c ordered_set.c parallel_io.c string_util.c openfoam_matrix.c rapl.c

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE =

# EXE += spmv_mkl_ie.exe
# EXE += spmv_mkl_coo.exe
# EXE += spmv_mkl_dia.exe
# EXE += spmv_mkl_csc.exe
# EXE += spmv_mkl_csr.exe
# EXE += spmv_mkl_bsr_2.exe spmv_mkl_bsr_4.exe spmv_mkl_bsr_8.exe spmv_mkl_bsr_16.exe spmv_mkl_bsr_32.exe spmv_mkl_bsr_64.exe

# EXE += spmv_csr_naive.exe
# EXE += spmv_csr.exe
# EXE += spmv_csr_vector.exe
# EXE += spmv_csr_vector_x86.exe
# EXE += spmv_csr_vector_perfect_nnz_balance.exe
# EXE += spmv_csr_simd.exe
# EXE += spmv_csr_prefetch.exe
# EXE += spmv_csr_x86_queues.exe

# EXE += spmv_dia.exe
# EXE += spmv_ldu.exe
# EXE += spmv_ell.exe

EXE += spmv_aocl_optmv.exe

DIRS = obj

all: $(DIRS) $(EXE)


# %.exe: obj/%.o $(LIB_OBJ)
# $(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)


spmv_mkl_coo.exe: obj/spmv.o spmv_kernel_mkl_coo.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_dia.exe: obj/spmv.o spmv_kernel_mkl_dia.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_csc.exe: obj/spmv.o spmv_kernel_mkl_csc.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_csr.exe: obj/spmv.o spmv_kernel_mkl_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_ie.exe: obj/spmv.o spmv_kernel_mkl_ie.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_2.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=2' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_4.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=4' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_8.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=8' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_16.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=16' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_32.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=32' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_64.exe: obj/spmv.o spmv_kernel_mkl_bsr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'BLOCK_SIZE=64' $^ -o $@ $(LDFLAGS_MKL)

spmv_csr_naive.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'NAIVE' $^ -o $@ $(LDFLAGS)
spmv_csr.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)
spmv_csr_vector.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_VECTOR' $^ -o $@ $(LDFLAGS)
spmv_csr_vector_x86.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_VECTOR_X86' $^ -o $@ $(LDFLAGS)
spmv_csr_vector_perfect_nnz_balance.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' $^ -o $@ $(LDFLAGS)
spmv_csr_simd.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_SIMD' $^ -o $@ $(LDFLAGS)
spmv_csr_prefetch.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_PREFETCH' $^ -o $@ $(LDFLAGS)
spmv_csr_x86_queues.exe: obj/spmv.o spmv_kernel_csr.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) -D'CUSTOM_QUEUES' $^ -o $@ $(LDFLAGS)

spmv_dia.exe: obj/spmv.o spmv_kernel_dia.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)
spmv_ldu.exe: obj/spmv.o spmv_kernel_ldu.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)
spmv_ell.exe: obj/spmv.o spmv_kernel_ell.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)

spmv_aocl_optmv.exe: obj/spmv.o spmv_kernel_aocl_optmv.c $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_AOCL)
<<<<<<< HEAD
=======

>>>>>>> ff2b062cccaca2be7b6445af043b55b2667e1293

$(call Rule_Auto_Dependencies,'obj/spmv.o','spmv.cpp')
	$(CPP) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,'obj/read_mtx.o','read_mtx.cpp')
	$(CPP) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,'obj/artificial_matrix_generation.o','$(AMG_PATH)/artificial_matrix_generation.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/ordered_set.o','$(AMG_PATH)/ordered_set.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/string_util.o','$(library)/string_util.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/parallel_io.o','$(library)/parallel_io.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/openfoam_matrix.o','$(library)/file_formats/openfoam/openfoam_matrix.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/rapl.o','$(library)/monitoring/power/rapl.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/csr_converter.o','$(library)/aux/csr_converter.c')
	$(CC) $(CFLAGS) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o *.o *.exe a.out

