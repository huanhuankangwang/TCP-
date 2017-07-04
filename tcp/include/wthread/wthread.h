#ifndef  _WTHREAD_H_
#define _WTHREAD_H_

#include <pthread.h>


typedef void *(*PF_Start)(void*);


pthread_t wthread_create(pthread_t *pid,const pthread_attr_t *attr,
						 PF_Start start, void *arg);
int wthread_close(pthread_t pid);
int wthread_join(pthread_t pid);


#endif//_WTHREAD_H_
