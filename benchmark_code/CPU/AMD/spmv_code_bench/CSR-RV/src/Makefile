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


library = ${C_LIB_PATH}


CC = gcc
# CC = /various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/gcc
# CC = clang

CPP = g++
# CPP = /various/dgal/gcc/gcc-12.2.0/gcc_bin/bin/g++
# CPP = clang++


CFLAGS = -Wall -Wextra
# Tells the compiler to use pipes instead of temporary files (faster compilation, but uses more memory).
CFLAGS += -pipe
CFLAGS += -fopenmp

CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter

CFLAGS += -Wno-init-self


# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

# CFLAGS += -ffast-math

CFLAGS += -flto=auto
CFLAGS += -march=native
CFLAGS += -mavx
CFLAGS += -mavx2
CFLAGS += -mavx512f
CFLAGS += -mavx512vl
CFLAGS += -mavx512bw

CFLAGS += -D'LEVEL1_DCACHE_LINESIZE=$(shell getconf LEVEL1_DCACHE_LINESIZE)'
CFLAGS += -D'LEVEL1_DCACHE_SIZE=$(shell getconf LEVEL1_DCACHE_SIZE)'
CFLAGS += -D'LEVEL2_CACHE_SIZE=$(shell getconf LEVEL2_CACHE_SIZE)'
CFLAGS += -D'LEVEL3_CACHE_SIZE=$(shell getconf LEVEL3_CACHE_SIZE)'

CFLAGS += -I'$(library)'


LDFLAGS =
# The gnu extension for nested functions needs an executable stack ('noexecstack' causes a segmentation fault).
# Uncomment if nested functions are used.
# LDFLAGS += -z execstack
LDFLAGS += -lm


LIB_SRC =

LIB_OBJ := $(LIB_SRC)
LIB_OBJ := $(patsubst %.c,obj/%.o,$(LIB_OBJ))
LIB_OBJ := $(patsubst %.cpp,obj/%.o,$(LIB_OBJ))


EXE = csv_avx512.exe

all: $(EXE) | $(DIRS)


%.exe: obj/%.o $(LIB_OBJ)
	$(CPP) $(CFLAGS) $^ -o $@ $(LDFLAGS)


$(call Rule_Auto_Dependencies,obj/csv_avx512.o,csv_avx512.cpp)
	$(CPP) $(CFLAGS) -c $< -o $@



$(DIRS): %:
	mkdir -p $@

clean:
	$(RM) obj/*.o obj/*.d *.o *.exe *.so a.out

