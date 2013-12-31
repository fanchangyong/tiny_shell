#include "cmd.h"

int find_dir(char* dir,char* cmd)
{
	DIR* dp=opendir(dir);
	if(dp==NULL)
	{
		perror("opendir");
		return 0;
	}
	struct dirent *entry;
	while((entry=readdir(dp))!=NULL)
	{
		char* name=entry->d_name;
		if(strcmp(".",name)==0 || strcmp("..",name)==0)
		{
			continue;
		}
		else if(strcmp(cmd,name)==0)
		{
			return 1;
		}
	}
	return 0;
}

int find_cmd(char* cmd,char* path,int len)
{
	char* pathenv=getenv("PATH");
	char* dupstr=strdup(pathenv);
	char* token;
	while((token=strsep(&dupstr,":"))!=NULL)
	{
		if(find_dir(token,cmd))
		{
			int tokenlen=strlen(token);
			strncpy(path,token,strlen(token));
			if(token[tokenlen-1]!='/')
			{
				strncat(path,"/",1);
			}
			strncat(path,cmd,strlen(cmd));
			return 1;
		}
	}
	return 0;
}

int do_cmd(char** tokens,int len)
{
	char path[1024];
	bzero(path,1024);
	char* cmd=tokens[0];
	if(strcmp(cmd,"")==0)
	{
		return 0;
	}

	int execute=0;
	if(cmd[0]=='/')
	{
		execute=1;
		strncpy(path,cmd,strlen(cmd));
	}
	else
	{
		if(find_cmd(cmd,path,1024))
		{
			// found command
			execute=1;
		}
		else
		{
			execute=0;
			printf("cannot find cmd:%s\n",cmd);
		}
	}
	if(execute)
	{
		pid_t pid=fork();
		if(pid==-1)
		{
			perror("fork");
		}
		else if(pid==0)
		{
			// child
			// do the command
			char** args=&tokens[1];
			if(-1==execv(path,args))
			{
				perror("execv");
				exit(-1);
			}
		}
		else
		{
			// parent
			// to wait for child process 
			int stat;
			if(-1==waitpid(pid,&stat,0))
			{
				perror("waitpid");
				return -1;
			}
			/*if(stat!=0)*/
				/*printf("child stat:%d\n",stat);*/
		}
	}
	return 0;
}

int is_space(char c)
{
	switch(c)
	{
		case ' ':
			return 1;
		case '\t':
			return 1;
		case '\n':
			return 1;
		default:
			return 0;
	}
}

void replace_space(char* str)
{
	int len=strlen(str);
	char* copy=malloc(sizeof(char)*len+1);
	strncpy(copy,str,len);
	copy[len]='\0';
	int i=0;
	int j=0;
	while(copy[i]!='\0')
	{
		if( is_space(copy[i]) && 
				(is_space(copy[i+1])	|| copy[i+1]=='\0'))
		{
			// skipped
		}
		else if( is_space(copy[i]))
		{
			str[j++]=' ';
		}
		else
		{
			str[j++]=copy[i];
		}
		i++;
	}
	str[j]=0;
}

char** parse_cmd(char* str,int *len)
{
	char* dupstr=strdup(str);
	replace_space(dupstr);
	char** token=malloc(sizeof(char*)*MAX_ARGS);
	int i=0;
	while((token[i]=strsep(&dupstr," "))!=NULL)
	{
		i++;
	}
	token[i]=NULL;
	*len=i;
	return token;
}
