#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ringbuffer.h>


#define    MIN_SIZE_RINGBUF    (1024)

#define     RINGBUF_DEBUG(...)
//#define       RINGBUF_DEBUG  printf

PT_RingBuffer mallocRingBuffer(int len)
{
	if(len < MIN_SIZE_RINGBUF)
		len = MIN_SIZE_RINGBUF;
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
	int ret = 0;
	if(ringbuf->mReadPos == ringbuf->mWritePos)
	{
		ret = 1;
	}
	return ret;
}
int readString(PT_RingBuffer ringbuf,char *buf,int maxlen)
{
	char *pbuf = buf;
	int i;
	pthread_mutex_lock(&ringbuf->mMutex);
    RINGBUF_DEBUG("lock in readString mReadPos=%d mWritePos=%d \r\n",ringbuf->mReadPos,ringbuf->mWritePos);
	for(i=0;i< maxlen;i++)
	{
		if(isEmpty(ringbuf))
			break;
		
		*pbuf = ringbuf->mBuf[ringbuf->mReadPos++];
		pbuf++;
		ringbuf->mReadPos = ringbuf->mReadPos % ringbuf->mLen;
	}

    RINGBUF_DEBUG("unlock in readString\r\n");
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
		{    
            break;
        }
		
		ringbuf->mBuf[ringbuf->mWritePos++] = *pbuf;
		pbuf++;
		ringbuf->mWritePos = ringbuf->mWritePos % ringbuf->mLen;
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
		ringbuf->mReadPos = ringbuf->mReadPos % ringbuf->mLen;
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
		ringbuf->mWritePos = ringbuf->mWritePos % ringbuf->mLen;
		ret = 1;
	}
	pthread_mutex_unlock(&ringbuf->mMutex);

	return ret;
}

