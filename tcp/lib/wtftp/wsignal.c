#include <signal.h>

#include <wsignal.h>

void signal_handler(int signum)
{
	if(signum == SIGQUIT)
	{
		pthread_exit(NULL);
	}
}
int wsignal()
{
	signal(SIGQUIT,signal_handler);
	return 0;
}

int wpthreadkill(pthread_t pid)
{
	pthread_kill(pid,SIGQUIT);
	return 0;
}
