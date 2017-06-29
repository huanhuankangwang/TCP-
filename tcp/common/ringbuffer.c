#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ringbuffer.h>

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
	int i;
	pthread_mutex_lock(&ringbuf->mMutex);
	for(i=0;i< maxlen;i++)
	{
		if(isEmpty(ringbuf) == 0)
			break;
		*pbuf = ringbuf->mBuf[ringbuf->mReadPos++];
		pbuf++;
	}
	pthread_mutex_unlock(&ringbuf->mMutex);

	return i;
}
int writeString(PT_RingBuffer ringbuf,char *buf,int len)
{
	char *pbuf = buf;
	int i=0;
	pthread_mutex_lock(&ringbuf->mMutex);
	for(i=0;i<len;i++)
	{
		if(isFull(ringbuf) == 0)
			break;

		ringbuf->mBuf[ringbuf->mWritePos++] = *pbuf;
		pbuf++;
	}
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
