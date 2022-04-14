.phony: all clean

ifeq ($(ARMPL_DIR),)
  $(error "ARMPL_DIR not set, try loading the appropriate module or explicitly exporting the variable")
endif

CC := g++
CFLAGS ?=  -mcpu=native -I$(ARMPL_DIR)/include #-std=gnu99
LDFLAGS ?= -Wl,--no-as-needed -L$(ARMPL_DIR)/lib -larmpl_mp -lm -lgomp -lpthread -ldl -lpapi

EXPORT_PATH ?= LD_LIBRARY_PATH=$(ARMPL_DIR)/lib:$(LD_LIBRARY_PATH)

obj := o

######################################################################################################################################################
DOUBLE := 1
CFLAGS += -D'DOUBLE=$(DOUBLE)'

AMG_PATH = ../../../artificial-matrix-generator
OLD_PATH = ../../../mkl_code
CSRC_ART_MAT = $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/artificial_matrix_generation.c  $(AMG_PATH)/ordered_set.c

CFLAGS += -fopenmp -O3
CFLAGS += -I$(AMG_PATH) -I$(AMG_PATH)/lib
CFLAGS += -I$(OLD_PATH) -I$(OLD_PATH)/lib
CFLAGS += -D'DOUBLE=$(DOUBLE)'
# CFLAGS += -Wno-deprecated-declarations -Wno-write-strings -Wno-unused-result
# CFLAGS += -Wno-deprecated-declarations -Wno-deprecated-writable-strings -Wno-write-strings -Wno-unused-result
######################################################################################################################################################

.SUFFIXES: .c .$(obj) .exe .res .diff .expected

RESULTS := spmv_c_example.exe

all: spmv_c_example.exe 

spmv_c_example.exe: spmv_c_example.c $(CSRC_ART_MAT)
	$(CC) $^ $(CFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -f *.exe
