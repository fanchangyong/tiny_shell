.PHONY: all

SRC=ts.c cmd.c input.c builtin.c
HEADERS=$(SRC:.c=.h)
OBJ=$(SRC:.c=.o)

all:ts tags

ts:$(OBJ)
	cc -o $@ $^
	rm -f $(OBJ)

tags:ts
	ctags -R

.o.c:.h
