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
    pid = pthread_self();
    if(signum == SIGQUIT)
        pthread_exit(p);
}

static void *wthread_run(void*arg)
{
    PT_Wthread_Private wp =(PT_Wthread_Private)arg;
	do{
	    if(!wp)
	      break;

	    signal(SIGQUIT,sig_handler);

	    wp->start(wp->arg);
	}while(0);
	//never do return
	return NULL;
}

int wthread_create(WthreadHandle *handle,
						 PF_WthreadStart start, void *arg)
{
	int ret = -1;
    PT_Wthread_Private wp = NULL;
	

	do{
		if(!handle || !arg)
			break;
		wp = malloc(sizeof(T_Wthread_Private));
        if(!wp)
        {
            break;
        }
        
        wp->start = start;
        wp->arg   = arg;
		ret = pthread_create(&wp->pid,NULL,wthread_run,(void*)wp);
		if(ret < 0)
		{
			free(wp);
			wp = NULL;
			break;
		}
		ret = 0;
		*handle = wp;
	}while(0);
	
	return ret;
}

int wthread_close(WthreadHandle handle)
{
   int ret = -1;
   PT_Wthread_Private wp = (PT_Wthread_Private)handle;
   do
   {
       if(!wp)
       {
         break;
       }
	   
       pthread_kill(wp->pid,SIGQUIT);
	   free(wp);
	   wp = NULL;
       ret = 0;
   }while(0);

   return ret;
}

int wthread_join(WthreadHandle handle)
{
	PT_Wthread_Private wp = (PT_Wthread_Private)handle;
	if(handle == NULL)
		return -1;
	
    return pthread_join(wp->pid,NULL);
}

