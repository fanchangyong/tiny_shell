#include "input.h"
#include "builtin.h"
#include "cmd.h"

int prompt()
{
	char* line=NULL;
	size_t linecap = 0;
	size_t linelen = 0;
	for(;;)
	{
		printf("Tiny Shell # ");
		fflush(stdout);
		linelen=getline(&line,&linecap,stdin);
		if(linelen==-1)
		{
			perror("getline");
			return -1;
		}
		int len;
		char** tokens=parse_cmd(line,&len);
		/*char** tmp;*/
		/*for(tmp=tokens;*tmp!=NULL;tmp++)*/
			/*printf("token:%s\n",*tmp);*/
		if(proc_builtin(tokens,len))
		{
			// User has input a builtin command
			continue;
		}
		else
		{
			// Not builtin,Execute the command
			do_cmd(tokens,len);
		}
	}
}

