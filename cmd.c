#include "cmd.h"
#include "builtin.h"

#define MAX_ARG 10
#define MAX_PATH 1024

#define REDIRECT_NONE   0
#define REDIRECT_INPUT  1
#define REDIRECT_OUTPUT 4

void replace_space(char* str);

struct cmd_t
{
	char path[MAX_PATH];
	char** args;
	int arglen;
	int fdin;
	int fdout;
	int in_redirect;
	int out_redirect;
	char in_filename[MAX_PATH];
	char out_filename[MAX_PATH];
};

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

int find_cmd(char* cmd,char* path,int pathlen)
{
	if(strcmp(cmd,"")==0)
	{
		return 0;
	}
	if(cmd[0]=='/') // absolute path
	{
		strncpy(path,cmd,pathlen);
		return 1;
	}
	if(cmd[0]=='.' && cmd[1]=='/') // start of "./"
	{
		strncpy(path,cmd,pathlen);
		return 1;
	}
	char* pathenv=getenv("PATH");
	char* dupstr=strdup(pathenv);
	char* token;
	while((token=strsep(&dupstr,":"))!=NULL)
	{
		if(find_dir(token,cmd))
		{
			int tokenlen=strlen(token);
			strncpy(path,token,pathlen);
			if(token[tokenlen-1]!='/')
			{
				strncat(path,"/",1);
			}
			strncat(path,cmd,pathlen-strlen(path));
			return 1;
		}
	}
	return 0;
}

int proc_redirect(char* cmd,char* newcmd,char* filename,char delim)
{
	char* dupcmd=strdup(cmd);
	replace_space(dupcmd);
	char* p ;
	int ret=0;
	if((p=strchr(dupcmd,delim))!=NULL)
	{
		int len=p-dupcmd;

		strcpy(newcmd,dupcmd);
		newcmd[len]=0;

		char* p2;
		if(*(p+1)==' ')
		{
			p2=strchr(p+2,' ');	
		}
		else
		{
		  p2=strchr(p,' ');
		}
		if(p2!=NULL)
		{
			strcat(newcmd,p2);
		}

		strcpy(filename,p+1);
		if(p2!=NULL)
		{
			filename[p2-p-1]=0;
		}
		ret=1;
	}
	return ret;
}

void copy_str(char* src,char* pstart,char* pend)
{
	while(pstart<=pend)
	{
		*src++=*pstart++;
	}
}

int redirect(char* cmd,char* newcmd,char* infile,char* outfile,int* in,int* out)
{
	char* dupcmd=strdup(cmd);
	replace_space(dupcmd);
	char* p1;
	char* p2;
	char* fp1=NULL;
	char* fp2=NULL;
	p1=strchr(dupcmd,'<');
	p2=strchr(dupcmd,'>');

	if(p1==NULL && p2==NULL)
	{
		*in=0;
		*out=0;
		return 0;
	}

	char* minp=NULL;
	char* maxp=NULL;

	if(p1==NULL)
	{
		fp1=outfile;
		minp=p2;
	}
	else if(p2==NULL)
	{
		fp1=infile;
		minp=p1;
	}
	else
	{
		if(p1<p2)
		{
			fp1=infile;
			fp2=outfile;
			minp=p1;
			maxp=p2;
		}
		else
		{
			fp1=outfile;
			fp2=infile;
			minp=p2;
			maxp=p1;
		}
	}

	// copy part 1 
	int cmdpart1;

	cmdpart1=minp-dupcmd;
	strcpy(newcmd,dupcmd);
	newcmd[cmdpart1]=0;
	printf("cmdpart1 is:%s\n",newcmd);

	// copy argument part
	char* cmdargp=NULL;
	
	char* pt;
	if(maxp!=NULL)
	{
		pt=minp;
	}
	else
	{
		pt=maxp;
	}

	if(*(pt+1)==' ')
	{
		cmdargp=strchr(pt+2,' ');
	}
	else
	{
		cmdargp=strchr(pt,' ');
	}

	if(cmdargp!=NULL)
	{
		strcat(newcmd,cmdargp);
	}

	printf("newcmd with arg is:%s\n",newcmd);

	// processing filenames
	char* file1start=NULL;
	char* file1end=NULL;
	char* file2start=NULL;
	char* file2end=NULL;

	// filename 1

	char* ptmp1;
	if(*(minp+1)==' ')
	{
		file1start=minp+2;
		ptmp1=strchr(minp+2,' ');
	}
	else
	{
		file1start=minp+1;
		ptmp1=strchr(minp,' ');
	}
	strcpy(fp1,file1start);
	printf("infile1:%s\n",infile);
	if(maxp!=NULL)
	{
		fp1[maxp-minp-1]=0;
	}
	else if(ptmp1!=NULL)
	{
		fp1[ptmp1-minp-1]=0;	
	}

	if(maxp!=NULL)
	{
		char* ptmp2;
		if(*(maxp+1)==' ')
		{
			ptmp2=strchr(maxp+2,' ');
		}
		else
		{
			ptmp2=strchr(maxp+1,' ');
		}
		if(fp2!=NULL)
		{
			strcpy(fp2,maxp+1);
			printf("fp2 is:%s,ptmp2 is:%s\n",fp2,ptmp2);
			if(ptmp2!=NULL)
			{
				fp2[ptmp2-maxp-1]=0;
			}
		}
	}
	return 1;
}

int do_cmds(char** cmds,int len)
{
	struct cmd_t* cmd_list=malloc(sizeof(struct cmd_t)*len);

	// create pipe fds
	int fds[len][2];	
	fds[0][0]=STDIN_FILENO;
	fds[len-1][1]=STDOUT_FILENO;
	int i;
	for(i=1;i<len;++i)
	{
		int fd[2];
		pipe(fd);
		fds[i][0]=fd[0];
		fds[i-1][1]=fd[1];
	}
	
	// process command arguments
	for(i=0;i<len;++i)
	{
		struct cmd_t* p=&cmd_list[i];

		// process redirection
		char* raw_cmd=strdup(cmds[i]);
		char* raw_cmd2=strdup(cmds[i]);
		char infilename[MAX_PATH];
		char outfilename[MAX_PATH];

		redirect(cmds[i],raw_cmd,p->in_filename,p->out_filename,
					   &p->in_redirect,&p->out_redirect);
		printf("raw_cmd:%s,in:%s,out:%s\n",raw_cmd,p->in_filename,p->out_filename);

		int arglen;
		char** args=parse_cmd(raw_cmd," ",&arglen);
		p->arglen=arglen;
		char* cmd=args[0];

		char path[1024];
		if(find_cmd(cmd,path,sizeof(path)))
		{
			strncpy(p->path,path,sizeof(p->path));
			p->args=args;
			p->fdin=fds[i][0];
			p->fdout=fds[i][1];
		}
		else if(is_builtin(cmd))
		{
			strncpy(p->path,cmd,sizeof(p->path));
			p->args=args;
			p->fdin=-1;
			p->fdout=-1;
		}
		else
		{
			// cannot find command
			if(strcmp(cmd,"")!=0)
				printf("cann't find command:%s\n",cmds[i]);
			return 0;
		}

	}

	// do action
	for(i=0;i<len;++i)
	{
		struct cmd_t* p=&cmd_list[i];
		if(proc_builtin(p->args,p->arglen))
		{
			// nothing to do
			continue;
		}
		
		pid_t pid=fork();
		if(pid<0)
		{
			perror("fork");
			return 0;
		}
		else if(pid==0)
		{
			if(p->out_redirect==1)
			{
				int fd=open(p->out_filename,O_WRONLY|O_CREAT,0700);
				if(fd==-1)
				{
					perror(p->args[0]);
					exit(1);
				}
				p->fdout=fd;
			}
			if(p->in_redirect==1)
			{
				int fd=open(p->in_filename,O_RDONLY);
				if(fd==-1)
				{
					perror(p->args[0]);
					exit(1);
				}
				p->fdin=fd;
			}

			if(p->fdin!=STDIN_FILENO)
			{
				dup2(p->fdin,STDIN_FILENO);
			}
			if(p->fdout!=STDOUT_FILENO)
			{
				dup2(p->fdout,STDOUT_FILENO);
			}
			int j;
			for(j=0;j<len;++j)
			{
				if(fds[j][0]!=STDIN_FILENO) close(fds[j][0]);
				if(fds[j][1]!=STDOUT_FILENO) close(fds[j][1]);
			}
			execv(p->path,p->args);
			perror("execv");
		}
	}

	int j;
	for(j=0;j<len;++j)
	{
		if(fds[j][0]!=STDIN_FILENO) close(fds[j][0]);
		if(fds[j][1]!=STDOUT_FILENO) close(fds[j][1]);
	}

	for(j=0;j<len;++j)
	{
		int stat;
		wait(&stat);
	}
	
	return 1;
}

// Abandonded.
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
			char** args=&tokens[0];
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
		if(is_space(copy[i]))
		{
			if(j==0)
			{
				// discard		
			}
			else if(is_space(copy[i+1]))
			{
				// discard
			}
			else if(copy[i+1]=='\0')
			{
				// discard
			}
			else if(copy[i+1]=='<')
			{
			}
			else if(copy[i-1]=='<')
			{}
			else if(copy[i-1]=='>')
			{
			}
			else if(copy[i-1]=='>')
			{}
			else
			{
				str[j++]=' ';
			}
		}
		else
		{
			str[j++]=copy[i];
		}
		i++;
	}
	str[j]=0;
}

char** parse_cmd(char* str,char* delim,int *len)
{
	char* dupstr=strdup(str);
	replace_space(dupstr);
	char** token=malloc(sizeof(char*)*MAX_ARGS);
	int i=0;
	while((token[i]=strsep(&dupstr,delim))!=NULL)
	{
		i++;
	}
	token[i]=NULL;
	*len=i;
	return token;
}
