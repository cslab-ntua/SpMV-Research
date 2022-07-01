.phony: all clean

CPATH = 
define NEWLINE


endef

shell_out := $(shell ../config.sh)
shell_out := $(subst ;,$(NEWLINE),$(shell_out))
$(eval $(shell_out))

AMG_PATH = ../../../../artificial-matrix-generator
LIB_PATH = ../../../../lib

CFLAGS = 
LDFLAGS =

executables = 
executables += spmv_csr_naive.exe
executables += spmv_armpl.exe

all: $(executables)

# CC=g++
# CC=clang++
CC=armclang++
CC2=g++

DOUBLE := 1

# https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu
CFLAGS += -Wall -Wno-deprecated-declarations -mcpu=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)'
# CFLAGS += -Wall -Wno-deprecated-declarations -march=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)'
CFLAGS += -I'$(AMG_PATH)' -I'$(LIB_PATH)'
CFLAGS += -armpl=lp64,parallel #-Rpass=.* -Rpass-analysis=.*

CFLAGS2 = -Wall -Wno-deprecated-declarations -march=native -fopenmp -O3 -D'DOUBLE=$(DOUBLE)' -I'$(AMG_PATH)' -I'$(LIB_PATH)' 

LDFLAGS += -Wl,--no-as-needed -lm #-lpapi
LDFLAGS += -lgomp -lpthread -ldl 
LDFLAGS += -larmpl_lp64_mp

DEPS  = $(AMG_PATH)/artificial_matrix_generation.c $(AMG_PATH)/ordered_set.c
DEPS += $(LIB_PATH)/monitoring/power/rapl.c

# spmv_csr_naive.exe: spmv.cpp $(DEPS)
# 	$(CC) $^ $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -o $@ $(LDFLAGS)

# spmv_armpl.exe: spmv_armpl.c $(DEPS)
# 	$(CC) $^ $(CFLAGS) -I$(ARMPL_DIR)/include -o $@ $(LDFLAGS) -L$(ARMPL_DIR)/lib

spmv_csr_naive.o: spmv.cpp
	$(CC) $(CFLAGS) -D'USE_CUSTOM_CSR' -D'NAIVE' -c $^ -o $@

spmv_armpl.o: spmv_armpl.c
	$(CC) $(CFLAGS) -I$(ARMPL_DIR)/include -c $^ -o $@

artificial_matrix_generation.o: $(AMG_PATH)/artificial_matrix_generation.c
	$(CC2) $(CFLAGS2) -c $^ -o $@

ordered_set.o: $(AMG_PATH)/ordered_set.c
	$(CC2) $(CFLAGS2) -c $^ -o $@

spmv_csr_naive.exe: spmv_csr_naive.o artificial_matrix_generation.o ordered_set.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

spmv_armpl.exe: spmv_armpl.o artificial_matrix_generation.o ordered_set.o
	$(CC) $(CFLAGS) -I$(ARMPL_DIR)/include $^ -o $@ $(LDFLAGS) -L$(ARMPL_DIR)/lib

clean:
	rm -f *.exe *.o
