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
CFLAGS += -fopenmp

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
# CFLAGS += -I'/usr/include/mkl'

# DOUBLE := 0
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'

# PROC_BENCH = 1
ifdef PROC_BENCH
    CFLAGS += -D'PROC_BENCH' 
endif


LDFLAGS =
LDFLAGS += -lm
LDFLAGS += -L'$(MKL_PATH)/lib/intel64'
LDFLAGS += -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl -lpapi


LIB_SRC = artificial_matrix_generation.c ordered_set.c parallel_io.c string_util.c openfoam_matrix.c

LIB_OBJ = $(patsubst %.c,obj/%.o,$(LIB_SRC))


EXE =
# EXE += spmv_coo.exe
# EXE += spmv_dia.exe
# EXE += spmv_csc.exe
# EXE += spmv_csr.exe
EXE += spmv_sparse_mv.exe
# EXE += spmv_bsr_2.exe spmv_bsr_4.exe spmv_bsr_8.exe spmv_bsr_16.exe spmv_bsr_32.exe spmv_bsr_64.exe
EXE += spmv_csr_naive.exe
EXE += spmv_csr_custom.exe
EXE += spmv_csr_custom_vector.exe
EXE += spmv_csr_custom_vector_perfect_nnz_balance.exe
# EXE += spmv_dia_custom.exe
EXE += spmv_ldu.exe
EXE += spmv_ell.exe

DIRS = obj

all: $(DIRS) $(EXE)


%.exe: obj/%.o $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)


obj/spmv_coo.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_COO' -c $< -o $@
obj/spmv_dia.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_DIA' -c $< -o $@
obj/spmv_csc.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_CSC' -c $< -o $@
obj/spmv_csr.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_CSR' -c $< -o $@
obj/spmv_sparse_mv.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_IE' -c $< -o $@
obj/spmv_bsr_2.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=2' -c $< -o $@
obj/spmv_bsr_4.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=4' -c $< -o $@
obj/spmv_bsr_8.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=8' -c $< -o $@
obj/spmv_bsr_16.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=16' -c $< -o $@
obj/spmv_bsr_32.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=32' -c $< -o $@
obj/spmv_bsr_64.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_MKL_BSR' -D'BLOCK_SIZE=64' -c $< -o $@
obj/spmv_csr_naive.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -c $< -o $@
obj/spmv_csr_custom.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_CUSTOM_CSR' -c $< -o $@
obj/spmv_csr_custom_vector.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_CUSTOM_CSR' -D'CUSTOM_VECTOR' -c $< -o $@
obj/spmv_csr_custom_vector_perfect_nnz_balance.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_CUSTOM_CSR' -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' -c $< -o $@
obj/spmv_ldu.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_LDU' -c $< -o $@
obj/spmv_ell.o: spmv.cpp
	$(CPP) $(CFLAGS) -D'USE_ELL' -c $< -o $@


$(call Rule_Auto_Dependencies,'obj/artificial_matrix_generation.o','$(AMG_PATH)/artificial_matrix_generation.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/ordered_set.o','$(AMG_PATH)/ordered_set.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/parallel_io.o','$(library)/parallel_io.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/string_util.o','$(library)/string_util.c')
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,'obj/openfoam_matrix.o','$(library)/file_formats/openfoam/openfoam_matrix.c')
	$(CC) $(CFLAGS) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o *.o *.exe a.out

