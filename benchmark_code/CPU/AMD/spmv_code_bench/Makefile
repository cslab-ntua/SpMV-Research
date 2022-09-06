.PHONY: all clean

SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables

# Targets that don't generate dependency files.
NODEPS = clean

define Rule_Auto_Dependencies_base =
    $(1:.o=.d): $(2)
	@echo 'Generating dependencies file:  $(2)'
	gcc $$(CFLAGS) -MT '$(1:.o=.d)' -MM -MG '$(2)' -MF '$(1:.o=.d)'
    ifeq (0, $(words $(findstring $(MAKECMDGOALS),$(NODEPS))))
        include $(1:.o=.d)
    endif
    $(1): $(1:.o=.d)
endef

define Rule_Auto_Dependencies =
    $(eval $(call Rule_Auto_Dependencies_base,$(1),$(2)))
    $(1): $(2)
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
# CC = xlc

CPP = g++
# CPP = clang++
# CPP = xlc++


ARCH = $(shell uname -m)


CFLAGS = -Wall -Wextra
CFLAGS += -pipe  # Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
# CFLAGS += -Wno-unused-variable
CFLAGS += -fopenmp

# CFLAGS += -g
# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

CFLAGS += -flto
ifeq ($(ARCH), x86_64)
    CFLAGS += -march=native
else
    CFLAGS += -mcpu=native
endif

CFLAGS += -I'$(library)'
CFLAGS += -I'$(AMG_PATH)'

CFLAGS += -D'INT_T=int32_t'

# DOUBLE := 0
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'

# PROC_BENCH = 1
ifdef PROC_BENCH
    CFLAGS += -D'PROC_BENCH' 
endif

ifeq ($(CPP), xlc++)
    CFLAGS += -std=c++11
endif


LDFLAGS =
LDFLAGS += -lm


LIB_SRC = read_mtx.cpp csr_converter.c artificial_matrix_generation.c ordered_set.c parallel_io.c string_util.c openfoam_matrix.c rapl.c

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE =

EXE += spmv_csr_naive.exe
EXE += spmv_csr.exe
EXE += spmv_csr_prefetch.exe
EXE += spmv_csr_simd.exe
EXE += spmv_csr_vector.exe
#
EXE += spmv_csr_x86_vector.exe
EXE += spmv_csr_x86_vector_queues.exe
EXE += spmv_csr_x86_vector_perfect_nnz_balance.exe

# EXE += spmv_dia.exe
# EXE += spmv_ldu.exe
# EXE += spmv_ell.exe

# EXE += spmv_mkl_ie.exe
# EXE += spmv_mkl_coo.exe
# EXE += spmv_mkl_dia.exe
# EXE += spmv_mkl_csc.exe
# EXE += spmv_mkl_csr.exe
# EXE += spmv_mkl_bsr_2.exe spmv_mkl_bsr_4.exe spmv_mkl_bsr_8.exe spmv_mkl_bsr_16.exe spmv_mkl_bsr_32.exe spmv_mkl_bsr_64.exe

# EXE += spmv_aocl_optmv.exe

# EXE += spmv_csr5.exe

# EXE += spmv_merge.exe


DIRS = obj

all: $(DIRS) $(EXE)


# %.exe: obj/%.o $(LIB_OBJ)
# $(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)


# x86_64 aarch64 ppc64le
CFLAGS_CUSTOM = $(CFLAGS)
ifeq ($(ARCH), x86_64)
    CFLAGS_CUSTOM += -mavx2
    # CFLAGS_CUSTOM += -mavx512f
else ifeq ($(ARCH), aarch64)
endif

spmv_csr_naive.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'NAIVE' $^ -o $@ $(LDFLAGS)
spmv_csr.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) $^ -o $@ $(LDFLAGS)
spmv_csr_prefetch.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_PREFETCH' $^ -o $@ $(LDFLAGS)
spmv_csr_simd.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_SIMD' $^ -o $@ $(LDFLAGS)
spmv_csr_vector.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_VECTOR' $^ -o $@ $(LDFLAGS)

spmv_csr_x86_vector.exe: obj/spmv_bench.o spmv_kernel_csr_x86.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_X86_VECTOR' $^ -o $@ $(LDFLAGS)
spmv_csr_x86_vector_perfect_nnz_balance.exe: obj/spmv_bench.o spmv_kernel_csr_x86.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_X86_VECTOR_PERFECT_NNZ_BALANCE' $^ -o $@ $(LDFLAGS)
spmv_csr_x86_vector_queues.exe: obj/spmv_bench.o spmv_kernel_csr_x86.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_X86_VECTOR_QUEUES' $^ -o $@ $(LDFLAGS)

spmv_ldu.exe: obj/spmv_bench.o spmv_kernel_ldu.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) $^ -o $@ $(LDFLAGS)
spmv_ell.exe: obj/spmv_bench.o spmv_kernel_ell.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) $^ -o $@ $(LDFLAGS)
spmv_dia.exe: obj/spmv_bench.o spmv_kernel_dia.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)


CFLAGS_MKL = $(CFLAGS)
CFLAGS_MKL += -I'$(MKL_PATH)/include' -I'/usr/include/mkl'
CFLAGS_MKL += -Wno-deprecated-declarations -m64 -mavx2

LDFLAGS_MKL = $(LDFLAGS)
LDFLAGS_MKL += -L'$(MKL_PATH)/lib/intel64'
LDFLAGS_MKL += -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -ldl

spmv_mkl_coo.exe: obj/spmv_bench.o spmv_kernel_mkl_coo.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_dia.exe: obj/spmv_bench.o spmv_kernel_mkl_dia.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_csc.exe: obj/spmv_bench.o spmv_kernel_mkl_csc.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_csr.exe: obj/spmv_bench.o spmv_kernel_mkl_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_ie.exe: obj/spmv_bench.o spmv_kernel_mkl_ie.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_2.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=2' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_4.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=4' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_8.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=8' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_16.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=16' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_32.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=32' $^ -o $@ $(LDFLAGS_MKL)
spmv_mkl_bsr_64.exe: obj/spmv_bench.o spmv_kernel_mkl_bsr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MKL) -D'BLOCK_SIZE=64' $^ -o $@ $(LDFLAGS_MKL)


CFLAGS_AOCL = $(CFLAGS)
CFLAGS_AOCL += -I'$(AOCL_PATH)/include' -I'$(AOCL_PATH)/../../library/src/include'
CFLAGS_AOCL += -m64 -mavx2

LDFLAGS_AOCL = $(LDFLAGS)
LDFLAGS_AOCL += -L'$(AOCL_PATH)/lib/'
LDFLAGS_AOCL += -Wl,--no-as-needed  -laoclsparse -lgomp -lpthread -ldl

spmv_aocl_optmv.exe: obj/spmv_bench.o spmv_kernel_aocl_optmv.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_AOCL) $^ -o $@ $(LDFLAGS_AOCL)


CFLAGS_CSR5 = $(CFLAGS)
CFLAGS_CSR5 += -Wno-deprecated-writable-strings -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare -Wno-unknown-pragmas -Wno-write-strings -Wno-unused-result

spmv_csr5.exe: obj/spmv_bench.o spmv_kernel_csr5.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CSR5) $^ -o $@ $(LDFLAGS)


CFLAGS_MERGE = $(CFLAGS)
CFLAGS_MERGE += -D'CUB_MKL'
CFLAGS_MERGE += -m64 -mavx2
CFLAGS_MERGE += -Wno-deprecated-declarations -Wno-sign-compare

LDFLAGS_MERGE = $(LDFLAGS)
LDFLAGS_MERGE += -lnuma -lrt -Wl,--no-as-needed -lgomp -lpthread -lm -ldl

spmv_merge.exe: obj/spmv_bench.o spmv_kernel_merge.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MERGE) $^ -o $@ $(LDFLAGS_MERGE)


$(call Rule_Auto_Dependencies,obj/spmv_bench.o,spmv_bench.cpp)
	$(CPP) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/read_mtx.o,read_mtx.cpp)
	$(CPP) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/artificial_matrix_generation.o,$(AMG_PATH)/artificial_matrix_generation.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/ordered_set.o,$(AMG_PATH)/ordered_set.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/string_util.o,$(library)/string_util.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/parallel_io.o,$(library)/parallel_io.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/openfoam_matrix.o,$(library)/file_formats/openfoam/openfoam_matrix.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/rapl.o,$(library)/monitoring/power/rapl.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/csr_converter.o,$(library)/aux/csr_converter.c)
	$(CC) $(CFLAGS) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o obj/*.d *.o *.exe a.out

