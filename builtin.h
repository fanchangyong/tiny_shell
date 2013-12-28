#include "common.h"

typedef void* (*func)(void*);

// builtin function implementations
void* ts_exit(void* arg);
void* ts_cd(void* arg);

struct cmdfunc
{
	char* name;
	func  handler;
};



int proc_builtin(char* cmd);

