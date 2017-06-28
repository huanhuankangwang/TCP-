#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <pthread.h>

/*
	********************ringbuffer*******************
	
*/

typedef struct _ringBuffer{
	unsigned char  *mBuf;
	int     mLen;
	int     mReadPos;
	int  	mWritePos;
	pthread_mutex_t mMutex;
}T_RingBuffer,*PT_RingBuffer;


PT_RingBuffer mallocRingBuffer(int len);
int		freeRingBuffer(PT_RingBuffer ringbuf);
int readString(PT_RingBuffer ringbuf,char *buf,int maxlen);
int writeString(PT_RingBuffer ringbuf,char *buf,int len);

int readChar(PT_RingBuffer ringbuf,char *ch);
int writeChar(PT_RingBuffer ringbuf,char ch);

#endif//_RING_BUFFER_H_