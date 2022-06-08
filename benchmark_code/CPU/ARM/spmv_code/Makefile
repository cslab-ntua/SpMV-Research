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
executables += spmv_csr_naive.exe
executables += spmv_armpl.exe

all: $(executables)

# CC=g++
CC=armclang++

DOUBLE := 1

# https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu
CFLAGS += -Wall -Wno-deprecated-declarations -mcpu=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)'
CFLAGS += -Rpass=.* -Rpass-analysis=.* -armpl=lp64,parallel
CFLAGS += -Ilib -I$(AMG_PATH) -I$(AMG_PATH)/lib

LDFLAGS ?= -Wl,--no-as-needed -larmpl_lp64_mp -lm -lgomp -lpthread -ldl -lpapi

spmv_csr_naive.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -o $@ $(LDFLAGS)

spmv_armpl.exe: spmv_armpl.c $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -I$(ARMPL_DIR)/include -o $@ $(LDFLAGS) -L$(ARMPL_DIR)/lib

clean:
	rm -f *.exe
