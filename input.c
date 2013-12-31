#include "input.h"
#include "builtin.h"
#include "cmd.h"
#include <readline/readline.h>

extern int errno;

int prompt()
{
	const char prompt_str[]="Tiny Shell # ";
	char* line;
	for(;;)
	{
		line=readline(prompt_str);
		if(!line)
			return -1;
		add_history(line);
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

