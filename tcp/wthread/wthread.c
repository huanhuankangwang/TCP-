#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "wthread_private.h"
#include <wthread.h>


typedef void *(*PF_Start)(void*);



void sig_handler(int signum) 
{
    //exit(0);
    void *p = NULL;
    pthread_t  pid;
	printf("pthread_exit \r\n");
    pid = pthread_self();
    if(signum == SIGQUIT)
        pthread_exit(p);
    closeWthreadPrivate(pid);
}


static void *wthread_run(void*arg)
{
    PT_Wthread_Private wp =(PT_Wthread_Private)arg;
    if(!wp)
       return NULL;

    signal(SIGQUIT,sig_handler);
    printf("run in wthread_run\r\n");

    wp->start(wp->arg);
    //¹Ø±ÕÏß³Ì
    memset(wp,0,sizeof(T_Wthread_Private));
}

pthread_t wthread_create(pthread_t *pid,const pthread_attr_t *attr,
						 PF_Start start, void *arg)
{
	int ret = -1;
    PT_Wthread_Private wp = NULL;
	do{
        wp = isCanOpen();
        if(!wp)
        {
            ret = -1;
            break;
        }
        
        wp->start = start;
        wp->arg   = arg;
		ret = pthread_create(pid,attr,wthread_run,(void*)wp);
		if(ret < 0)
		{
			break;
		}
        wp->pid = *pid;
	}while(0);

    printf("wthread_create pid=%d\r\n",*(int*)pid);
	
	return ret;
}

int wthread_close(pthread_t pid)
{
   int ret = -1;
   do
   {
       if(pid < 0)
       {
         return -1;
       }

       if( isExistWthread(pid) != 0)
            break;
       pthread_kill(pid,SIGQUIT);
       ret = 0;
   }while(0);

   return 0;
}

int wthread_join(pthread_t pid)
{
    if( isExistWthread(pid) == 0)
        return pthread_join(pid,NULL);
    
    return -1;
}

