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


CFLAGS =
CFLAGS += -Wall -Wextra
CFLAGS += -pipe  # Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
# CFLAGS += -Wno-unused-variable
CFLAGS += -fopenmp

# CFLAGS += -g
# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

# CFLAGS += -ffast-math

CFLAGS += -flto
ifeq ($(ARCH), x86_64)
    CFLAGS += -march=native
else ifeq ($(ARCH), ppc64le)
    CFLAGS += -mcpu=power9
else
    CFLAGS += -mcpu=native
endif

CFLAGS += -I'$(library)'
CFLAGS += -I'$(AMG_PATH)'

CFLAGS += -D'INT_T=int32_t'

# DOUBLE := 0
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'


CPPFLAGS =
CPPFLAGS += $(CFLAGS)

ifeq ($(CC), xlc)
    CFLAGS += -Wno-typedef-redefinition
    CFLAGS += -D'__XLC__'
endif
ifeq ($(CPP), xlc++)
    CPPFLAGS += -std=c++11
    CPPFLAGS += -D'__XLC__'
endif

CFLAGS += -D'LEVEL1_DCACHE_LINESIZE=$(shell getconf LEVEL1_DCACHE_LINESIZE)'
CFLAGS += -D'LEVEL1_DCACHE_SIZE=$(shell getconf LEVEL1_DCACHE_SIZE)'
CFLAGS += -D'LEVEL2_CACHE_SIZE=$(shell getconf LEVEL2_CACHE_SIZE)'
CFLAGS += -D'LEVEL3_CACHE_SIZE=$(shell getconf LEVEL3_CACHE_SIZE)'


LDFLAGS =
LDFLAGS += -lm


LIB_SRC = read_mtx.cpp csr_converter.c artificial_matrix_generation.c ordered_set.c pthread_functions.c omp_functions.c array_metrics.c string_util.c parallel_io.c openfoam_matrix.c rapl.c

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE =

EXE += spmv_csr_naive.exe
EXE += spmv_csr.exe
EXE += spmv_csr_kahan.exe
# EXE += spmv_csr_prefetch.exe
# EXE += spmv_csr_simd.exe
# EXE += spmv_csr_vector.exe
EXE += spmv_csr_vector_perfect_nnz_balance.exe

EXE += spmv_csr_vc.exe

EXE += spmv_csr_x86_vector.exe
# EXE += spmv_csr_x86_vector_queues.exe
EXE += spmv_csr_x86_vector_perfect_nnz_balance.exe

# EXE += spmv_dia.exe
# EXE += spmv_ldu.exe
# EXE += spmv_ell.exe

EXE += spmv_mkl_ie.exe
# EXE += spmv_mkl_coo.exe
# EXE += spmv_mkl_dia.exe
# EXE += spmv_mkl_csc.exe
# EXE += spmv_mkl_csr.exe
# EXE += spmv_mkl_bsr_2.exe spmv_mkl_bsr_4.exe spmv_mkl_bsr_8.exe spmv_mkl_bsr_16.exe spmv_mkl_bsr_32.exe spmv_mkl_bsr_64.exe

# EXE += spmv_aocl_optmv.exe

EXE += spmv_csr5.exe

EXE += spmv_merge.exe

EXE += spmv_sparsex.exe


DIRS = obj

all: $(DIRS) $(EXE)


# x86_64 aarch64 ppc64le
CFLAGS_CUSTOM = $(CPPFLAGS)
ifeq ($(ARCH), x86_64)
    CFLAGS_CUSTOM += -mavx2
    # CFLAGS_CUSTOM += -mavx512f
else ifeq ($(ARCH), aarch64)
endif

spmv_csr_naive.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'NAIVE' $^ -o $@ $(LDFLAGS)
spmv_csr.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) $^ -o $@ $(LDFLAGS)
spmv_csr_kahan.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_KAHAN' $^ -o $@ $(LDFLAGS)
spmv_csr_prefetch.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_PREFETCH' $^ -o $@ $(LDFLAGS)
spmv_csr_simd.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_SIMD' $^ -o $@ $(LDFLAGS)
spmv_csr_vector.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_VECTOR' $^ -o $@ $(LDFLAGS)
spmv_csr_vector_perfect_nnz_balance.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' $^ -o $@ $(LDFLAGS)

spmv_csr_vc.exe: obj/spmv_bench.o spmv_kernel_csr_vc.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CUSTOM) -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' $^ -o $@ $(LDFLAGS)

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


CFLAGS_MKL = $(CPPFLAGS)
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


CFLAGS_AOCL = $(CPPFLAGS)
CFLAGS_AOCL += -I'$(AOCL_PATH)/include' -I'$(AOCL_PATH)/../../library/src/include'
CFLAGS_AOCL += -m64 -mavx2

LDFLAGS_AOCL = $(LDFLAGS)
LDFLAGS_AOCL += -L'$(AOCL_PATH)/lib/'
LDFLAGS_AOCL += -Wl,--no-as-needed  -laoclsparse -lgomp -lpthread -ldl

spmv_aocl_optmv.exe: obj/spmv_bench.o spmv_kernel_aocl_optmv.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_AOCL) $^ -o $@ $(LDFLAGS_AOCL)


CFLAGS_CSR5 = $(CPPFLAGS)
CFLAGS_CSR5 += -Wno-deprecated-writable-strings -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare -Wno-unknown-pragmas -Wno-write-strings -Wno-unused-result

spmv_csr5.exe: obj/spmv_bench.o spmv_kernel_csr5.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_CSR5) $^ -o $@ $(LDFLAGS)


CFLAGS_MERGE = $(CPPFLAGS)
# CFLAGS_MERGE += -D'CUB_MKL'
CFLAGS_MERGE += -m64
ifeq ($(ARCH), x86_64)
    CFLAGS_MERGE += -mavx2
    # CFLAGS_MERGE += -mavx512f
else ifeq ($(ARCH), aarch64)
endif
CFLAGS_MERGE += -Wno-deprecated-declarations -Wno-sign-compare
ifeq ($(CPP), xlc++)
    CFLAGS_MERGE += -Wno-unknown-attributes
    CFLAGS_MERGE += -Wno-unused-parameter
    CFLAGS_MERGE += -Wno-unused-variable
endif

LDFLAGS_MERGE = $(LDFLAGS)
LDFLAGS_MERGE += -lrt -Wl,--no-as-needed -lgomp -lpthread -lm -ldl
# LDFLAGS_MERGE += -lnuma

spmv_merge.exe: obj/spmv_bench.o spmv_kernel_merge.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS_MERGE) $^ -o $@ $(LDFLAGS_MERGE)


CPPFLAGS_SPARSEX = $(CPPFLAGS)
CPPFLAGS_SPARSEX += -Wno-unused-variable
CPPFLAGS_SPARSEX += -Wno-unused-but-set-variable
CPPFLAGS_SPARSEX += -Wno-unused-parameter
CPPFLAGS_SPARSEX += -Wno-sign-compare
CPPFLAGS_SPARSEX += -Wno-unused-local-typedefs
CPPFLAGS_SPARSEX += -Wno-deprecated-copy
CPPFLAGS_SPARSEX += -Wno-placement-new
CPPFLAGS_SPARSEX += -Wno-deprecated-declarations
CPPFLAGS_SPARSEX += -Wno-parentheses
CPPFLAGS_SPARSEX += -fopenmp
CPPFLAGS_SPARSEX += -I'$(BOOST_INC_PATH)'
CPPFLAGS_SPARSEX += -I'$(LLVM_INC_PATH)'
CPPFLAGS_SPARSEX += -I$(SPARSEX_INC_PATH)
CPPFLAGS_SPARSEX += $(shell ${SPARSEX_CONF_PATH}/sparsex-config --cppflags)

LDFLAGS_SPARSEX = $(LDFLAGS)
LDFLAGS_SPARSEX += $(shell ${SPARSEX_CONF_PATH}/sparsex-config --ldflags)
LDFLAGS_SPARSEX += -L'$(BOOST_LIB_PATH)'
LDFLAGS_SPARSEX += -L'$(LLVM_LIB_PATH)'
LDFLAGS_SPARSEX += -L'$(SPARSEX_LIB_PATH)'
LDFLAGS_SPARSEX += -lboost_regex
LDFLAGS_SPARSEX += -lboost_serialization
LDFLAGS_SPARSEX += -lboost_system
LDFLAGS_SPARSEX += -lboost_atomic
# LDFLAGS_SPARSEX += -lnuma
# LDFLAGS_SPARSEX += -lz -ltinfo -lrt -lgomp -lpthread -ldl -lpapi -fopenmp

# This needs to be put FIRST.
LDFLAGS_SPARSEX := -Wl,--no-as-needed $(LDFLAGS_SPARSEX)

spmv_sparsex.exe: obj/spmv_bench.o obj/spmv_kernel_sparsex.o $(LIB_OBJ)
	$(CPP) $(CPPFLAGS_SPARSEX) $^ -o $@ $(LDFLAGS_SPARSEX)

obj/spmv_kernel_sparsex.o: spmv_kernel_sparsex.cpp
	$(CPP) $(CPPFLAGS_SPARSEX) -c $< -o $@



$(call Rule_Auto_Dependencies,obj/spmv_bench.o,spmv_bench.cpp)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/read_mtx.o,read_mtx.cpp)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/artificial_matrix_generation.o,$(AMG_PATH)/artificial_matrix_generation.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/ordered_set.o,$(AMG_PATH)/ordered_set.c)
	$(CC) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/pthread_functions.o,$(library)/pthread_functions.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/omp_functions.o,$(library)/omp_functions.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/array_metrics.o,$(library)/array_metrics.c)
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

