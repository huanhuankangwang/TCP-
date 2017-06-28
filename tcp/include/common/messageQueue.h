#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <pthread.h>
#include "messageRecord.h"



typedef struct _MessageQueue 
{
   pthread_mutex_t   cond_lock;
   //pthread_cond_t    cond;
   MessageRecord* fHead;
   MessageRecord* fTail;
   int          mLen;
}MessageQueue ;


int init_messageQueue(MessageQueue *queue);
MessageQueue *malloc_messageQueue();
int free_messageQueue(MessageQueue *queue) ;
void enqueue(MessageQueue *queue,MessageRecord* record);
MessageRecord* dequeue(MessageQueue *queue) ;
void putAtHead(MessageQueue *queue,MessageRecord* record) ;
MessageRecord* findByCSeq(MessageQueue *queue,unsigned cseq);

int  getQueueLength(MessageQueue *queue);
MessageRecord* removeOneByCseq(MessageQueue * queue,int cseq);


#endif /* _MESSAGE_QUEUE_H_ */
