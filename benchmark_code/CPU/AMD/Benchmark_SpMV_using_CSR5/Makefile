
CPATH = 

#compilers
CC=c++

# VALUE_TYPE = float
VALUE_TYPE = double

NUM_RUN = 128

library = ${C_LIB_PATH}


AMG_PATH = ../../../../../artificial-matrix-generator
CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/ordered_set.c

CFLAGS =
CFLAGS += -I$(AMG_PATH)
CFLAGS += -I "$(library)"
CFLAGS += -Wno-deprecated-writable-strings
CFLAGS += -Wno-write-strings
CFLAGS += -Wno-unused-result

#ENVIRONMENT_PARAMETERS
#OPTIONS=-xCORE-AVX2 -opt-prefetch=3 #icc/icpc option
OPTIONS=-mavx -mavx2 -march=native #c++ option
#backup
#$(CC) -xCORE-AVX2 -opt-prefetch=3 -Wno-deprecated-writable-strings -fopenmp -O3 main.cpp -o spmv -D VALUE_TYPE=$(VALUE_TYPE) -D NUM_RUN=$(NUM_RUN)

avx2: main.cpp $(CSRC_ART_MAT)
	$(CC) $(OPTIONS) $(CFLAGS) -fopenmp -O3 $^ -o spmv -D VALUE_TYPE=$(VALUE_TYPE) -D NUM_RUN=$(NUM_RUN)

clean:
	$(RM) spmv

