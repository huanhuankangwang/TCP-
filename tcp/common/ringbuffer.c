#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ringbuffer.h>

#define     RINGBUF_DEBUG(...)
//#define       RINGBUF_DEBUG  printf

PT_RingBuffer mallocRingBuffer(int len)
{
	PT_RingBuffer buf = malloc(sizeof(T_RingBuffer));

	do
	{
		if(!buf)
			break;
		buf->mBuf = malloc(sizeof(char) *(len+1));
		if(!buf->mBuf)
		{
			free(buf);
			buf = NULL;
			break;
		}

		 buf->mLen = len;
		 buf->mReadPos = 0;
		 buf->mWritePos= 0;
		 pthread_mutex_init(&buf->mMutex, NULL);
	}while(0);

	return buf;
}


int	freeRingBuffer(PT_RingBuffer ringbuf)
{
	if(ringbuf)
	{
		free(ringbuf->mBuf);
        ringbuf->mBuf = NULL;
		free(ringbuf);
		ringbuf = NULL;
	}

	return 0;
}

int isFull(PT_RingBuffer ringbuf)
{
	int ret = 1;
	if((ringbuf->mWritePos + 1)% ringbuf->mLen == ringbuf->mReadPos)
	{
		ret = 0;
	}
	return ret;
}

int isEmpty(PT_RingBuffer ringbuf)
{
	int ret = 1;
	if(ringbuf->mReadPos == ringbuf->mWritePos)
	{
		ret = 0;
	}
	return ret;
}
int readString(PT_RingBuffer ringbuf,char *buf,int maxlen)
{
	char *pbuf = buf;
	char *mbuf = ringbuf->mBuf;
	int i;
	pthread_mutex_lock(&ringbuf->mMutex);
    printf("lock in readString mReadPos=%d mWritePos=%d \r\n",ringbuf->mReadPos,ringbuf->mWritePos);
	for(i=0;i< maxlen;i++)
	{
	
    	printf("for mReadPos=%d mWritePos=%d \r\n",ringbuf->mReadPos,ringbuf->mWritePos);
		if(isEmpty(ringbuf) == 0)
			break;
		
    	printf("for *pbuf=%d \r\n",*pbuf);
		*pbuf = mbuf[ringbuf->mReadPos];
    	printf("for mReadPos=%d mWritePos=%d \r\n",ringbuf->mReadPos,ringbuf->mWritePos);

		ringbuf->mReadPos++;
		
    	printf("for mReadPos=%d mWritePos=%d \r\n",ringbuf->mReadPos,ringbuf->mWritePos);
		pbuf++;
	}

    printf("unlock in readString\r\n");
	pthread_mutex_unlock(&ringbuf->mMutex);

	return i;
}
int writeString(PT_RingBuffer ringbuf,char *buf,int len)
{
	char *pbuf = buf;
	int i=0;
	
	pthread_mutex_lock(&ringbuf->mMutex);
    RINGBUF_DEBUG("lock in writeString\r\n");
	for(i=0;i<len;i++)
	{
		if(isFull(ringbuf) == 0)
			break;
		
		printf("mWritePos =%d mReadPos =%d\r\n",ringbuf->mWritePos,ringbuf->mReadPos);
		ringbuf->mBuf[ringbuf->mWritePos++] = *pbuf;
		printf("ch =%c i=%d\r\n",*pbuf,i);
		pbuf++;
		printf("ch =%c i=%d\r\n",*pbuf,i);
	}

    RINGBUF_DEBUG("unlock in writeString\r\n");
	pthread_mutex_unlock(&ringbuf->mMutex);

	return i;
}

int readChar(PT_RingBuffer ringbuf,char *ch)
{
	int ret = 0;
	pthread_mutex_lock(&ringbuf->mMutex);
	if(isEmpty(ringbuf) != 0)
	{
		*ch = ringbuf->mBuf[ringbuf->mReadPos++];
		ret = 1;
	}
	pthread_mutex_unlock(&ringbuf->mMutex);

	return ret;
}
int writeChar(PT_RingBuffer ringbuf,char ch)
{
	int ret = 0;

	pthread_mutex_lock(&ringbuf->mMutex);
	if(isFull(ringbuf) != 0)
	{
		ringbuf->mBuf[ringbuf->mWritePos++] = ch;
		ret = 1;
	}
	pthread_mutex_unlock(&ringbuf->mMutex);

	return ret;
}

