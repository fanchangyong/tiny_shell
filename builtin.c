#include "builtin.h"

struct cmdfunc (builtin_cmds[2])=
{
	{"exit\n",ts_exit},
	{"cd\n",ts_cd}
};

#define BUILTIN_CMD_COUNT (sizeof(builtin_cmds)/sizeof(struct cmdfunc))

int proc_builtin(char* cmd)
{
	int i;
	for(i=0;i<BUILTIN_CMD_COUNT;++i)
	{
		struct cmdfunc cf=builtin_cmds[i];
		if(strcmp(cf.name,cmd)==0)
		{
			cf.handler("");
		}
	}
	return 0;
}

void* ts_exit(void* arg)
{
	int status=*(int*)arg;
	exit(status);
}

void* ts_cd(void* arg)
{
	printf("todo cd\n");
	return 0;
}
