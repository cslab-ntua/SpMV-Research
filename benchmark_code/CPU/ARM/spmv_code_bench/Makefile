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
	armclang $$(CFLAGS) -MT '$(1:.o=.d)' -MM -MG '$(2)' -MF '$(1:.o=.d)'
    ifeq (0, $(words $(findstring $(MAKECMDGOALS),$(NODEPS))))
        include $(1:.o=.d)
    endif
    $(1): $(1:.o=.d)
endef

define Rule_Auto_Dependencies =
    $(eval $(call Rule_Auto_Dependencies_base,$(1),$(2)))
    $(1): $(2)
endef

# # 'shell' merges all lines into one, but gcc -M ends each line with '\' (backslash), so we have to remove them.
# define Rule_Auto_Dependencies =
#     $(info Searching dependencies: $(2))
#     $(shell armclang $(CFLAGS) -MT '$(1)' -MM -MG '$(2)' | sed 's/\\$$//g')
# endef

library = ../../../../lib


CPATH = 
define NEWLINE


endef

shell_out := $(shell ../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

AMG_PATH = ../../../../artificial-matrix-generator


CC = gcc
# CC = armclang
# CC = clang

# CPP = g++
CPP = armclang++
# CPP = clang++
CPP2 = g++


ARCH = $(shell uname -m)


CFLAGS = -Wall -Wextra
CFLAGS += -pipe  # Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
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
ifeq ($(ARCH), x86_64)
    CFLAGS += -march=native
else
    CFLAGS += -mcpu=native
endif

CFLAGS += -armpl=lp64,parallel -Rpass=.* -Rpass-analysis=.* -fsave-optimization-record 

CFLAGS += -I'$(library)'
CFLAGS += -I'$(AMG_PATH)'
CFLAGS += -D'INT_T=int32_t'

# DOUBLE := 0
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'

CFLAGS2 = -Wall -Wno-deprecated-declarations -march=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)' -D'INT_T=int32_t' -I'$(AMG_PATH)' -I'$(library)' 

# PROC_BENCH = 1
ifdef PROC_BENCH
    CFLAGS += -D'PROC_BENCH' 
endif


LDFLAGS =
LDFLAGS += -Wl,--no-as-needed -lm #-lpapi
LDFLAGS += -lgomp -lpthread -ldl 
LDFLAGS += -larmpl_lp64_mp

LIB_SRC = read_mtx.cpp csr_converter.c artificial_matrix_generation.c ordered_set.c parallel_io.c string_util.c openfoam_matrix.c rapl_arm.c

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE =

EXE += spmv_csr_naive.exe
# EXE += spmv_csr.exe
# EXE += spmv_csr_vector.exe
# EXE += spmv_csr_vector_x86.exe
# EXE += spmv_csr_vector_perfect_nnz_balance.exe
# EXE += spmv_csr_simd.exe
# EXE += spmv_csr_prefetch.exe
# EXE += spmv_csr_x86_queues.exe

# EXE += spmv_ldu.exe
# EXE += spmv_ell.exe

EXE += spmv_armpl.exe

EXE += spmv_merge.exe

DIRS = obj

all: $(DIRS) $(EXE)


# %.exe: obj/%.o $(LIB_OBJ)
# $(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_MKL)


# spmv_csr_naive.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'NAIVE' $^ -o $@ $(LDFLAGS)
spmv_csr_naive.exe: obj/spmv_bench2.o spmv_kernel_csr.cpp $(LIB_OBJ)
	$(CPP2) $(CFLAGS2) -D'NAIVE' $^ -o $@ $(LDFLAGS)

# spmv_csr.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)
# spmv_csr_vector.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'CUSTOM_VECTOR' $^ -o $@ $(LDFLAGS)
# spmv_csr_vector_x86.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'CUSTOM_VECTOR_X86' $^ -o $@ $(LDFLAGS)
# spmv_csr_vector_perfect_nnz_balance.exe: obj/spmv_bench2.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP2) $(CFLAGS2) -D'CUSTOM_VECTOR_PERFECT_NNZ_BALANCE' $^ -o $@ $(LDFLAGS)
# spmv_csr_simd.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'CUSTOM_SIMD' $^ -o $@ $(LDFLAGS)
# spmv_csr_prefetch.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'CUSTOM_PREFETCH' $^ -o $@ $(LDFLAGS)
# spmv_csr_x86_queues.exe: obj/spmv_bench.o spmv_kernel_csr.cpp $(LIB_OBJ)
# 	$(CPP) $(CFLAGS) -D'CUSTOM_QUEUES' $^ -o $@ $(LDFLAGS)

spmv_ldu.exe: obj/spmv_bench2.o spmv_kernel_ldu.cpp $(LIB_OBJ)
	$(CPP2) $(CFLAGS2) $^ -o $@ $(LDFLAGS)
spmv_ell.exe: obj/spmv_bench2.o spmv_kernel_ell.cpp $(LIB_OBJ)
	$(CPP2) $(CFLAGS2) $^ -o $@ $(LDFLAGS)

spmv_armpl.exe: obj/spmv_bench.o spmv_kernel_armpl.cpp $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS_AOCL)

CFLAGS_MERGE = $(CFLAGS2)
# CFLAGS_MERGE += -D'CUB_MKL'
# CFLAGS_MERGE += -m64 -mavx2
CFLAGS_MERGE += -Wno-sign-compare

LDFLAGS_MERGE = $(LDFLAGS)
LDFLAGS_MERGE += -lnuma -lrt -Wl,--no-as-needed -lgomp -lpthread -lm -ldl

spmv_merge.exe: obj/spmv_bench2.o spmv_kernel_merge.cpp $(LIB_OBJ)
	$(CPP2) $(CFLAGS_MERGE) $^ -o $@ $(LDFLAGS_MERGE)


$(call Rule_Auto_Dependencies,obj/spmv_bench.o,spmv_bench.cpp)
	$(CPP) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/spmv_bench2.o,spmv_bench.cpp)
	$(CPP2) $(CFLAGS2) -c $< -o $@

# $(call Rule_Auto_Dependencies,obj/read_mtx.o,read_mtx.cpp)
# 	$(CPP) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/read_mtx.o,read_mtx.cpp)
	$(CC) $(CFLAGS2) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/artificial_matrix_generation.o,$(AMG_PATH)/artificial_matrix_generation.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/ordered_set.o,$(AMG_PATH)/ordered_set.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/string_util.o,$(library)/string_util.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/parallel_io.o,$(library)/parallel_io.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/openfoam_matrix.o,$(library)/file_formats/openfoam/openfoam_matrix.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/rapl_arm.o,$(library)/monitoring/power/rapl_arm.c)
	$(CC) $(CFLAGS2) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/csr_converter.o,$(library)/aux/csr_converter.c)
	$(CC) $(CFLAGS2) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o obj/*.d *.o *.exe a.out


