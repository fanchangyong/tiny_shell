#include "input.h"
#include "builtin.h"


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
		proc_builtin(line);	

		// do command

	}
}

