#ifndef  _WTHREAD_H_
#define _WTHREAD_H_

#include <pthread.h>


typedef void *(*PF_WthreadStart)(void*);
typedef void* 	WthreadHandle;


int wthread_create(WthreadHandle *handle,
						 PF_WthreadStart start, void *arg);
int wthread_close(WthreadHandle handle);
int wthread_join(WthreadHandle handle);


#endif//_WTHREAD_H_
