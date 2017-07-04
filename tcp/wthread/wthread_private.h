#ifndef _WTHREAD_PRIVATE_H_
#define _WTHREAD_PRIVATE_H_

#include <pthread.h>
typedef void *(*PF_Start)(void*);


typedef struct wthread_private{
    pthread_t     pid;
	PF_Start      start;
	void          *arg;
    int           isRunning;
    int           flags;//used 1 not used 0    
}T_Wthread_Private,*PT_Wthread_Private;

int closeWthreadPrivate(int pid);
PT_Wthread_Private isCanOpen();

#endif//_WTHREAD_PRIVATE_H_
