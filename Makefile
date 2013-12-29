.PHONY: all

SRC=ts.c cmd.c input.c builtin.c
HEADERS=cmd.h common.h input.h builtin.h
OBJ=ts.o cmd.o input.o builtin.o

all:ts tags

ts:$(OBJ)
	cc -o $@ $^
	rm -f $(OBJ)

tags:ts
	ctags -R

.o.c:.h
