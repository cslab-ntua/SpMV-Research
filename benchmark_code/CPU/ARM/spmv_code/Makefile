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
# executables += spmv_armpl.exe

all: $(executables)

# CC=g++
# CC=clang++
CC=armclang++

DOUBLE := 1

# https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu
CFLAGS += -Wall -Wno-deprecated-declarations -mcpu=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)'
CFLAGS += -fopenmp
CFLAGS += -D'DOUBLE=$(DOUBLE)'

CFLAGS += -O3
CFLAGS += -mcpu=native

CFLAGS += -I'../../../../lib' -I$(AMG_PATH) -I$(AMG_PATH)/lib

CFLAGS += -armpl=lp64,parallel -Rpass=.* -Rpass-analysis=.*

LDFLAGS =
LDFLAGS += -Wl,--no-as-needed -lm -lpapi
LDFLAGS += -lgomp -lpthread -ldl

LDFLAGS += -larmpl_lp64_mp


spmv_csr_naive.exe: spmv.cpp $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -o $@ $(LDFLAGS)

spmv_armpl.exe: spmv_armpl.c $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -I$(ARMPL_DIR)/include -o $@ $(LDFLAGS) -L$(ARMPL_DIR)/lib

clean:
	rm -f *.exe
