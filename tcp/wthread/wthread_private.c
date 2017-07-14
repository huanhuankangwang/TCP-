#include "wthread_private.h"

#include <string.h>

#define    MAX_SIZE_WTHREAD             (50)
#define    WTHREAD_PRIVATED_USED        (1)


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static T_Wthread_Private   wthreadPrivate[MAX_SIZE_WTHREAD]={0};

int closeWthreadPrivate(int pid)
{
    pthread_mutex_lock(&mutex);
    void *wp = NULL;
    int i = 0;
    for(i=0;i< MAX_SIZE_WTHREAD;i++)
    {
        if( wthreadPrivate[i].pid == pid && wthreadPrivate[i].flags == WTHREAD_PRIVATED_USED )
        {
            wp = (void*)&wthreadPrivate[i];
            memset(wp,0,sizeof(T_Wthread_Private));
            return 0;
        }
    }
    
    pthread_mutex_unlock(&mutex);
    return -1;
}

PT_Wthread_Private isCanOpen()
{
    pthread_mutex_lock(&mutex);
    int i;
    for(i=0;i< MAX_SIZE_WTHREAD;i++)
    {
        if(wthreadPrivate[i].flags != WTHREAD_PRIVATED_USED )
        {
            pthread_mutex_unlock(&mutex);
            return &wthreadPrivate[i];
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int isExistWthread(pthread_t pid)
{
    pthread_mutex_lock(&mutex);
    void *wp = NULL;
    int i = 0;
    for(i=0;i< MAX_SIZE_WTHREAD;i++)
    {
        if( wthreadPrivate[i].pid == pid && wthreadPrivate[i].flags == WTHREAD_PRIVATED_USED )
        {
            return 0;
        }
    }
    
    pthread_mutex_unlock(&mutex);
    return -1;
}

