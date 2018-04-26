#ifndef  _WTHREAD_H_
#define _WTHREAD_H_

#include <pthread.h>


typedef void *(*PF_WthreadStart)(void*);
typedef void 	WthreadHandle;


int wthread_create(WthreadHandle *pid,
						 PF_WthreadStart start, void *arg);
int wthread_close(pthread_t pid);
int wthread_join(pthread_t pid);


#endif//_WTHREAD_H_
