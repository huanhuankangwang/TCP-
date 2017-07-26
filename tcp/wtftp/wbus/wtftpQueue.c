#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>

#include <wtftpQueue.h>


PT_WtftpQueue malloc_wtftpQueue() {
    PT_WtftpQueue.queue = (PT_WtftpQueue)malloc(sizeof(T_WtftpQueue));
    if(queue) {
        memset(queue,0,sizeof(T_WtftpQueue));
        queue->fHead = NULL;
        queue->fTail = NULL;
        queue->mLen  = 0;
        pthread_mutex_init(&queue->cond_lock, NULL);
        //pthread_cond_init(&queue->cond, NULL);
    }
    return queue;
}

int free_wtftpRecord(PT_WtftpRecord record) {
    record->mNext = NULL;
    free(record);
    return 0;
}

int free_wtftpQueue(PT_WtftpQueue queue) {
    pthread_mutex_lock(&queue->cond_lock);
    PT_WtftpRecord tail = queue->fHead;
    PT_WtftpRecord temp = NULL;
    while(tail) {
        temp = tail->fNext;
        free_wtftpRecord(tail);
        tail = temp;
    }

    queue->mLen  = 0;
    queue->fHead = NULL;
    queue->fTail = NULL;
    pthread_mutex_unlock(&queue->cond_lock);
    pthread_mutex_destroy(&queue->cond_lock);
    free(queue);
    queue = NULL;
    return 0;
}


PT_WtftpRecord malloc_wtftpRecord(BusMsg *msg,pFunWtftpCallback funCb) {
    PT_WtftpRecord  record = NULL;

    do {
        if(msg == NULL) {
            break;
        }

        record = malloc(sizeof(T_WtftpRecord));
        if(record == NULL) {
            break;
        }

        memcpy(record->mMsg,msg,sizeof(BusMsg));
        record->mCb = funCb;
        record->mNext = NULL;
    } while(0);

    return record;
}

int wtftp_equeue(PT_WtftpQueue  queue,PT_WtftpRecord  record) {
    if(queue == NULL || record == NULL) {
        return -1;
    }
    pthread_mutex_lock(&queue->cond_lock);
    if (queue->fHead== NULL) {
        queue->fHead = record;
    } else {
        queue->fTail->fNext = record;
    }
    queue->fTail  = record;
    record->fNext = NULL;
    queue->mLen++;
    pthread_mutex_unlock(&queue->cond_lock);

    return 0;
}

PT_WtftpRecord wtftp_dequeue(PT_WtftpQueue queue) {
    PT_WtftpRecord record = NULL;
    if(!queue) {
        return record;
    }

    pthread_mutex_lock(&queue->cond_lock);
    record = queue->fHead;

    if (queue->fHead == queue->fTail) {
        queue->fHead = NULL;
        queue->fTail = NULL;
    } else {
        queue->fHead = queue->fHead->fNext;
    }
    if (record != NULL) {
        queue->mLen--;
        record->fNext = NULL;
    }
    pthread_mutex_unlock(&queue->cond_lock);

    return record;
}

void putAtHead(PT_WtftpRecord queue,PT_WtftpRecord record) {
    if(!queue || !record)
        return ;
    pthread_mutex_lock(&queue->cond_lock);
    record->fNext = queue->fHead;
    queue->fHead = record;
    if (queue->fTail == NULL) {
        queue->fTail = record;
    }
    queue->mLen++;
    pthread_mutex_unlock(&queue->cond_lock);
}

PT_WtftpRecord findByCSeq(PT_WtftpRecord queue,int cseq) {
    pthread_mutex_lock(&queue->cond_lock);
    PT_WtftpRecord record = NULL;
    for (record = queue->fHead; record != NULL; record = record->fNext ) {
        if (record->mMsg.mCseq == cseq) {
            pthread_mutex_unlock(&queue->cond_lock);
            return record;
        }
    }

    pthread_mutex_unlock(&queue->cond_lock);
    return NULL;
}


