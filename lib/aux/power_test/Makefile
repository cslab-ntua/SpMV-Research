.PHONY: all clean

SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables

# 'shell' merges all lines into one, but gcc -M ends each line with '\' (backslash), so we have to remove them.
define Rule_Auto_Dependencies =
    $(shell gcc $(CFLAGS) -MT '$(1)' -MM '$(2)' | sed 's/\\$$//g')
endef

# library = ${C_LIB_PATH}
library = ../../


# CC = gcc
CC = g++


CFLAGS = -Wall -Wextra
# CFLAGS += -g3 -fno-omit-frame-pointer
# CFLAGS += -Og
# CFLAGS += -O0
# CFLAGS += -O2
CFLAGS += -O3

CFLAGS += -flto
CFLAGS += -march=native


CFLAGS += -I '$(library)'


LDFLAGS =


LIB = rapl.c

LIB_OBJ = $(patsubst %.c,obj/%.o,$(LIB))


EXE = rapl_test.exe

DIRS = obj

all: $(DIRS) $(EXE)


%.exe: obj/%.o $(LIB_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(call Rule_Auto_Dependencies,'obj/rapl_test.o','rapl_test.c')
	$(CC) $(CFLAGS) -c $< -o $@


$(call Rule_Auto_Dependencies,'obj/rapl.o','$(library)/monitoring/power/rapl.c')
	$(CC) $(CFLAGS) -c $< -o $@

# obj/rapl_test.o: rapl_test.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# obj/rapl.o: $(library)/monitoring/power/rapl.c
# 	$(CC) $(CFLAGS) -c $< -o $@


$(DIRS): %:
	mkdir -p $@

clean: 
	$(RM) *.o *.exe a.out obj/*.o

