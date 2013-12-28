.PHONY: all

SRC=main.c cmd.c input.c builtin.c
HEADERS=cmd.h common.h input.h builtin.h
OBJ=main.o cmd.o input.o builtin.o

all:ts

ts:$(OBJ)
	cc -o $@ $^
	rm -f $(OBJ)

.o.c:.h
