#include "input.h"
#include "builtin.h"
#include "cmd.h"
#include <readline/readline.h>

extern int errno;

int prompt()
{
	signal(SIGINT,SIG_IGN);
	const char prompt_str[]="Tiny Shell # ";
	char* line;
	for(;;)
	{
		line=readline(prompt_str);
		if(!line)
			return -1;
		add_history(line);
		int len;
		char** cmds=parse_cmd(line,"|",&len);

		do_cmds(cmds,len);
	}
}

