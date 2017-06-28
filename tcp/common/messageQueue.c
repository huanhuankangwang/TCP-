#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "messageQueue.h"

#define  LOGE(...)
//#define    LOGE       printf


int init_messageQueue(MessageQueue *queue)
{
	if(queue)
	{
	    memset(queue,0,sizeof(MessageQueue));
		queue->fHead = NULL;
		queue->fTail = NULL;
        queue->mLen  = 0;
        pthread_mutex_init(&queue->cond_lock, NULL);
        //pthread_cond_init(&queue->cond, NULL);
	}
}

MessageQueue *malloc_messageQueue()
{
	MessageQueue *queue = malloc(sizeof(MessageQueue));
	if(queue)
	{
	    memset(queue,0,sizeof(MessageQueue));
		queue->fHead = NULL;
		queue->fTail = NULL;
        queue->mLen  = 0;
        pthread_mutex_init(&queue->cond_lock, NULL);
        //pthread_cond_init(&queue->cond, NULL);
	}
	return queue;
}

int free_messageQueue(MessageQueue *queue)
{
    pthread_mutex_lock(&queue->cond_lock);
	MessageRecord * tail = queue->fHead;
	MessageRecord * temp = NULL;
	while(tail)
	{
		temp = tail->fNext;
		free_record(tail);
		tail = temp;
	}
	pthread_mutex_unlock(&queue->cond_lock);
	return 0;
}

void enqueue(MessageQueue *queue,MessageRecord* record)
{
    pthread_mutex_lock(&queue->cond_lock);
    if (queue->fTail == NULL) {
        queue->fHead = record;
    } else {
        queue->fTail->fNext = record;
    }
    queue->fTail = record;
    queue->mLen++;
    pthread_mutex_unlock(&queue->cond_lock);
}

MessageRecord* dequeue(MessageQueue *queue)
{
    pthread_mutex_lock(&queue->cond_lock);
    MessageRecord* record = queue->fHead;
    if (queue->fHead == queue->fTail) {
        queue->fHead = NULL;
        queue->fTail = NULL;
    } else {
        queue->fHead = queue->fHead->fNext;
    }
    if (record != NULL)
    {
        queue->mLen--;
        record->fNext = NULL;
    }
    pthread_mutex_unlock(&queue->cond_lock);

    return record;
}

void putAtHead(MessageQueue *queue,MessageRecord* record) 
{
    pthread_mutex_lock(&queue->cond_lock);

    record->fNext = queue->fHead;
    queue->fHead = record;
    if (queue->fTail == NULL) {
        queue->fTail = record;
    }
    pthread_mutex_unlock(&queue->cond_lock);
}


MessageRecord* removeOneByCseq(MessageQueue * queue,int cseq)
{
    MessageRecord *pp,*p,*ret=NULL;
    //
    pthread_mutex_lock(&queue->cond_lock);

    LOGE(" cseq =%d\r\n",cseq);

    for (p = pp = queue->fHead; p != NULL; pp = p , p = p->fNext ) 
	{
	    LOGE("loop p=%p\r\n",p);
        if (p->fCSeq == cseq)
        {
            LOGE("find cseq=%d\r\n",cseq);
            queue->mLen--;
            if(p == queue->fHead)
            {
                LOGE("rm head \r\n");
                if(p == queue->fTail)
                {
                    LOGE("rm head tail\r\n");
                    queue->fHead = queue->fTail = NULL;
                    
                }else{
                    LOGE("rm head others\r\n");
                    queue->fHead = queue->fHead->fNext;
                    LOGE("this way\r\n");
                }
                
            }else 
            {
                LOGE("rm others \r\n");
                pp->fNext = p->fNext;
            }

            //free_record(p);
            ret = p;
            ret->fNext = NULL;
            break;  
        }
    }
    
    pthread_mutex_unlock(&queue->cond_lock);   

    return ret;
}

MessageRecord* findByCSeq(MessageQueue *queue,unsigned cseq) 
{
    pthread_mutex_lock(&queue->cond_lock);
    MessageRecord* record;
    for (record = queue->fHead; record != NULL; record = record->fNext ) 
	{
        if (record->fCSeq == cseq)
        {
            pthread_mutex_unlock(&queue->cond_lock);
            return record;
        }
    }

    pthread_mutex_lock(&queue->cond_lock);
    return NULL;
}

int  getQueueLength(MessageQueue *queue)
{
    int len;
    pthread_mutex_lock(&queue->cond_lock);
    len = queue->mLen;
    pthread_mutex_unlock(&queue->cond_lock);
    return len;
}


MessageRecord *getByCSeq(MessageQueue *queue,int cseq)
{
    
}