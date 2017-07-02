#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>

#include <stdlib.h>



typedef void *(*PF_Start)(void*);

typedef struct Wthread_Arg
{
	PF_Start  start;
	void *arg;
}Wthread_Arg;

typedef struct{
	pthread_t pid;
	void *arg;
}T_WThread,*PT_WThread;



PT_WThread wthread_create(const pthread_attr_t *attr,
		void *(*start_routine) (void *), void *arg);

int pthread_close(PT_WThread thread)
{
	if(thread)
	{
		pthread_kill(thread->pid,SIGQUIT);
		//pthread_cancel(thread->pid);
	}

	usleep(20);

	free(thread);
	
	return 0;
}

void sig_handler(int signum) {
    //exit(0);
    void *p;
	printf("pthread_exit \r\n");
    pthread_exit(p);
}


static void *wthread_run(void*arg)
{
	Wthread_Arg *wArg = (Wthread_Arg*)arg;
	if(!wArg && !wArg->start)
		return NULL;
	
	signal(SIGQUIT,sig_handler);
	wArg->start(wArg->arg);
}
PT_WThread wthread_create(const pthread_attr_t *attr,
						 PF_Start start, void *arg)
{
	int ret = 0;
	PT_WThread thread = NULL;
	Wthread_Arg *wArg = NULL;
	do{
		thread = malloc(sizeof(T_WThread));
		if(!thread)
			break;

		thread->arg = malloc(sizeof(Wthread_Arg));
		if(!thread->arg)
		{
			free(thread);
			thread = NULL;
			break;
		}

		wArg = (Wthread_Arg*)(thread->arg);
		wArg->arg   = arg;
		wArg->start = start;
		
		ret = pthread_create(&thread->pid,attr,wthread_run,(void*)wArg);
		if(ret < 0)
		{
			free(thread);
			thread = NULL;
			break;
		}
	}while(0);
	
	return thread;
}


void *start(void*arg)
{
	do{
		sleep(1);
		printf("continue\r\n");
	}while(1);

	printf("exit start\r\n");
	
}

int main()
{
	char  tmp[100];
	
  	PT_WThread  thread = wthread_create(NULL,start,NULL);
	if(!thread)
	{
		printf("wthread_create error");
	}

	while(1)
	{
		memset(tmp,0,sizeof(tmp));

		scanf("%s",tmp);
		if(strcmp(tmp,"quit") == 0)
		{
			pthread_close(thread);
		}
	}
	return 0;
}

