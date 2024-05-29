.phony: all clean

CPATH = 
define NEWLINE


endef

shell_out := $(shell ../../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

all: cpu_spmv

AMG_PATH = ../../../../../artificial-matrix-generator
CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c $(AMG_PATH)/ordered_set.c
LIB_PATH = ../../../../../lib

#-------------------------------------------------------------------------------
# Compiler and compilation platform
#-------------------------------------------------------------------------------
CUB_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

# OMP compiler
CC=g++

DOUBLE = 1

CFLAGS  = -Wall -Wno-deprecated-declarations -fopenmp -O3 -march=native -D'DOUBLE=$(DOUBLE)'
CFLAGS += -m64 -mavx2 
CFLAGS += -I'$(AMG_PATH)' -I'$(LIB_PATH)'

LDFLAGS  = -lnuma -lrt -Wl,--no-as-needed -lgomp -lpthread -lm -ldl #-lpapi

# Includes
INC += -I$(CUB_DIR) -I$(CUB_DIR)test 

#-------------------------------------------------------------------------------
# Dependency Lists
#-------------------------------------------------------------------------------

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

DEPS = 	$(call rwildcard, $(CUB_DIR),*.cuh) \
		$(call rwildcard, $(CUB_DIR),*.h) \
        Makefile
DEPS2 = $(CSRC_ART_MAT)
DEPS2 += $(LIB_PATH)/monitoring/power/rapl.c

#-------------------------------------------------------------------------------
# make clean
#-------------------------------------------------------------------------------

clean :
	rm -f spmv_merge.exe

#-------------------------------------------------------------------------------
# make cpu_spmv
#-------------------------------------------------------------------------------

cpu_spmv : cpu_spmv.cpp $(DEPS) $(DEPS2)
	$(CC) $(DEFINES) -DCUB_MKL $(CFLAGS) -o spmv_merge.exe cpu_spmv.cpp $(DEPS2) $(LDFLAGS)
