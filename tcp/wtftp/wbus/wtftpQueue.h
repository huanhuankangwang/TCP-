#ifndef _WTFTP_QUEUE_H_
#define _WTFTP_QUEUE_H_

typedef int(*pFunWtftpCallback)(char *msgtype,char *msgData,int dataSize);

typedef struct _WtftpRecord {
    struct _WtftpRecord* mNext;
    BusMsg  mMsg;
    pFunWtftpCallback mCb;
} T_WtftpRecord,*PT_WtftpRecord;


typedef struct _WtftpQueue {
    pthread_mutex_t   lock;
    PT_WtftpRecord fHead;
    PT_WtftpRecord fTail;
    int          mLen;
} T_WtftpQueue,*PT_WtftpQueue;


PT_WtftpQueue malloc_wtftpQueue();
int free_wtftpRecord(PT_WtftpRecord record);
int free_wtftpQueue(PT_WtftpQueue queue);
PT_WtftpRecord malloc_wtftpRecord(BusMsg *msg,pFunWtftpCallback funCb);
int wtftp_equeue(PT_WtftpQueue  queue,PT_WtftpRecord  record);
PT_WtftpRecord wtftp_dequeue(PT_WtftpQueue queue);
void putAtHead(PT_WtftpRecord queue,PT_WtftpRecord record);
PT_WtftpRecord findByCSeq(PT_WtftpRecord queue,int cseq);

#endif//_WTFTP_QUEUE_H_