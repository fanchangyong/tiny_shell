#include "builtin.h"

struct cmdfunc (builtin_cmds[3])=
{
	{"exit",ts_exit},
	{"cd",ts_cd},
	{"pwd",ts_pwd},
};

#define BUILTIN_CMD_COUNT (sizeof(builtin_cmds)/sizeof(struct cmdfunc))

int proc_builtin(char** tokens,int len)
{
	char* cmd=tokens[0];
	char** args=&tokens[1];
	int i;
	for(i=0;i<BUILTIN_CMD_COUNT;++i)
	{
		struct cmdfunc cf=builtin_cmds[i];
		if(strcmp(cf.name,cmd)==0)
		{
			cf.handler(args,len-1);
			return 1;
		}
	}
	return 0;
}

void* ts_exit(char** arg,int len)
{
	exit(0);
}

void* ts_cd(char** arg,int len)
{
	char dir[256];
	bzero(dir,256);
	if(len>0)
	{
		int arglen=strlen(arg[0]);
		strncpy(dir,arg[0],arglen);
	}
	else
	{
		// Go Home
		struct passwd *pw=getpwuid(getuid());
		const char* homedir = pw->pw_dir;
		strncpy(dir,homedir,strlen(homedir));
	}
	if(-1==chdir(dir))
	{
		perror("chdir");
	}
	return 0;
}

void* ts_pwd(char** arg,int len)
{
	char* cwd=getcwd(NULL,0);
	printf("cwd:%s\n",cwd);
	return 0;
}
