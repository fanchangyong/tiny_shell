#include "common.h"

#define MAX_ARGS 1024

int do_cmd();
int do_cmds(char** cmds,int len);
char** parse_cmd(char* str,char* delim,int *len);
