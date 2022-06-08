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

executables = 
executables += avx2
all: $(executables)

#compilers
CC=c++

# VALUE_TYPE = float
VALUE_TYPE = double

NUM_RUN = 128


CFLAGS += -Wno-deprecated-writable-strings -Wno-write-strings -Wno-unused-result

CFLAGS += -I'$(AMG_PATH)' -I'$(AMG_PATH)/lib'


#ENVIRONMENT_PARAMETERS
#OPTIONS=-xCORE-AVX2 -opt-prefetch=3 #icc/icpc option
OPTIONS=-mavx -mavx2 -march=native #c++ option
#backup
#$(CC) -xCORE-AVX2 -opt-prefetch=3 -Wno-deprecated-writable-strings -fopenmp -O3 main.cpp -o spmv_csr5.exe -D VALUE_TYPE=$(VALUE_TYPE) -D NUM_RUN=$(NUM_RUN)

avx2: main.cpp $(CSRC_ART_MAT)
	$(CC) $(OPTIONS) $(CFLAGS) -fopenmp -O3 $^ -o spmv_csr5.exe -D VALUE_TYPE=$(VALUE_TYPE) -D NUM_RUN=$(NUM_RUN)

clean:
	$(RM) spmv_csr5.exe

