#include "common.h"

typedef void* (*func)(char**,int len);

// builtin function implementations
void* ts_exit(char** arg,int len);
void* ts_cd(char** arg,int len);
void* ts_pwd(char** arg,int len);

struct cmdfunc
{
	char* name;
	func  handler;
};



int proc_builtin(char** tokens,int len);

