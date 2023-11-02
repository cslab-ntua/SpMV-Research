.PHONY: all clean

SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables

# Targets that don't generate dependency files.
NODEPS = clean

DIRS = obj

define Rule_Auto_Dependencies_base =
    $(1:.o=.d): $(2) | $(DIRS)
	@echo 'Generating dependencies file:  $(2)'
	gcc $$(CFLAGS) -MT '$(1:.o=.d)' -MM -MG '$(2)' -MF '$(1:.o=.d)'
    ifeq (0, $(words $(findstring $(MAKECMDGOALS),$(NODEPS))))
        -include $(1:.o=.d)
    endif
    $(1): $(1:.o=.d)
endef

define Rule_Auto_Dependencies =
    $(eval $(call Rule_Auto_Dependencies_base,$(1),$(2)))
    $(1): $(2)
endef


library = ../../../lib


CPATH = 
define NEWLINE


endef

shell_out := $(shell ../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

AMG_PATH = ../../../artificial-matrix-generator


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
# CFLAGS += -Wno-alloc-size-larger-than
CFLAGS += -fopenmp

# CFLAGS += -g
# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

# CFLAGS += -ffast-math

CFLAGS += -flto=auto
ifeq ($(ARCH), x86_64)
    CFLAGS += -mbmi
    CFLAGS += -mbmi2
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

ifeq ($(DOUBLE),1)
    CFLAGS += -D'ValueType=double'
else
    CFLAGS += -D'ValueType=float'
endif

ifeq ($(PRINT_STATISTICS),1)
    CFLAGS += -D'PRINT_STATISTICS'
endif


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


LIB_SRC = pthread_functions.c omp_functions.c topology.c hash.c random.c hashtable.c bitstream.c bytestream.c array_metrics.c string_util.c io.c parallel_io.c plot.c openfoam_matrix.c rapl.c read_mtx.cpp csr_converter.c csr_converter_double.c csr_util.c artificial_matrix_generation.c ordered_set.c

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE =
EXE += spmv_optima.exe


all: $(EXE) | $(DIRS)


# x86_64 aarch64 ppc64le
CFLAGS_CUSTOM = $(CPPFLAGS)
ifeq ($(ARCH), x86_64)
    CFLAGS_CUSTOM += -mavx2
    # CFLAGS_CUSTOM += -mavx512f
else ifeq ($(ARCH), aarch64)
endif

# host compiler global settings
# CFLAGS_CUSTOM += -std=c++0x
CFLAGS_CUSTOM += -DVITIS_PLATFORM=$(VITIS_PLATFORM) -DCU=${CU} 
CFLAGS_CUSTOM += -D__USE_XOPEN2K8 -I$(XILINX_VIVADO)/include/ -I$(XILINX_VITIS_HLS)/include/ -I$(XILINX_XRT)/include/ 
LDFLAGS += -lxilinxopencl -lpthread -lrt -lstdc++ -L$(XILINX_XRT)/lib/ -Wl,-rpath-link,$(XILINX_XRT)/lib

spmv_optima.exe: obj/spmv_bench.o spmv_kernel_optima.cpp $(LIB_OBJ) ./obj/xcl2.o
	$(CPP) $(CFLAGS_CUSTOM) $^ -o $@ $(LDFLAGS)

$(call Rule_Auto_Dependencies,obj/spmv_bench.o,spmv_bench.cpp)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/xcl2.o,optima/optima_spmv/src/xcl2.cpp)
	$(CPP) $(CFLAGS_CUSTOM) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/read_mtx.o,read_mtx.c)
	$(CC) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/artificial_matrix_generation.o,$(AMG_PATH)/artificial_matrix_generation.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/ordered_set.o,$(AMG_PATH)/ordered_set.c)
	$(CC) $(CFLAGS) -c $< -o $@

$(call Rule_Auto_Dependencies,obj/pthread_functions.o,$(library)/pthread_functions.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/omp_functions.o,$(library)/omp_functions.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/topology.o,$(library)/topology.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/hash.o,$(library)/hash/hash.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/random.o,$(library)/random.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/hashtable.o,$(library)/aux/hashtable.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/bitstream.o,$(library)/bitstream.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/bytestream.o,$(library)/bytestream.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/array_metrics.o,$(library)/array_metrics.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/string_util.o,$(library)/string_util.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/io.o,$(library)/io.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/parallel_io.o,$(library)/parallel_io.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/plot.o,$(library)/plot/plot.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/openfoam_matrix.o,$(library)/storage_formats/openfoam/openfoam_matrix.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/rapl.o,$(library)/monitoring/power/rapl.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/csr_converter.o,$(library)/aux/csr_converter.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/csr_converter_double.o,$(library)/aux/csr_converter_double.c)
	$(CC) $(CFLAGS) -c $< -o $@
$(call Rule_Auto_Dependencies,obj/csr_util.o,$(library)/aux/csr_util.c)
	$(CC) $(CFLAGS) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o obj/*.d *.o *.exe a.out

