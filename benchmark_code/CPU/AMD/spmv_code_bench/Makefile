.PHONY: all clean

SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.ONESHELL:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKECMDGOALS ?=


all:
	./make.sh $(MAKECMDGOALS)


clean:
	$(RM) obj/* *.o *.exe a.out
	cd LCM-partially-strided-codelet
	make clean

