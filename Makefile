.PHONY: all

SRC=ts.c cmd.c input.c builtin.c ts_sig.c
HEADERS=$(SRC:.c=.h)
OBJ=$(SRC:.c=.o)

all:ts tags

ts:$(OBJ)
	cc -o $@ $^
	#rm -f $(OBJ)

tags:ts
	ctags -R

.PHONY:cl

# clean up
cl:
	rm -f $(OBJ) tags

.o.c:.h
