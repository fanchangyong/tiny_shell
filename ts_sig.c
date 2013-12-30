#include "ts_sig.h"

struct sig_fun
{
	int sig;
	void (*fun)(int sig);
};

void install_sig_action()
{
	struct sigaction sa;
	sa.sa_handler=sig_int_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;

	//sigaction(SIGINT,&sa,NULL);
	signal(SIGINT,SIG_IGN);
}

void sig_int_handler(int sig)
{
	printf("Caught signal,sig:%d\n",sig);
}
