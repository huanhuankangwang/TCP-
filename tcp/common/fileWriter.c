#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <fileWriter.h>
#include <fileoperation.h>

#include "config.h"


#define      RUNNING             1
#define      NOT_RUNNING         0
#define      RUNNING_QUIT       -1

#define      WRITE_FAILED        1
#define      WRITE_SUCCESS		 0


#define    IS_RUNNING(arg)   		(arg->isRunning == RUNNING)
#define    IS_WRITE_SUCCESS(arg)	(arg->flag == WRITE_SUCCESS)

//��С�Ļ��λ�������С
#define     MIN_BUFFSIZE        (1024*2)


int readFileWriter(PT_FileWriter writer,char *str,int maxsize);

void *do_write_thread(void*arg)
{
    int  ret = 0,len;
	char tmp[1024];
	PT_FileWriter writer = (PT_FileWriter)arg;	
	EB_LOGD("readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);
	do
	{
	    memset(tmp,0,sizeof(tmp));
		EB_LOGD("lock readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);

        pthread_mutex_lock(&writer->mutex);
		EB_LOGD("do_write_thread lock readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);
        ret = readFileWriter(writer,tmp,sizeof(tmp));//������������������
		EB_LOGD("do_write_thread ret =%d isRunn\r\n",ret,writer->isRunning);
        len = ret;
        ret = write_fd(writer->fd,tmp,len);
        if(ret != len)
        {
        	writer->flag = WRITE_FAILED;
            break;
        }
        
        pthread_mutex_unlock(&writer->mutex);
		EB_LOGD("do_write_thread unlock readFileWriter \r\n");	
	}while(writer->isRunning);

    writer->isRunning = RUNNING_QUIT;//�ɹ��˳�
}

PT_FileWriter openFileWriter(const char* filename,int bufSize)
{
	int ret;
	PT_FileWriter writer = NULL;

    if(bufSize < MIN_BUFFSIZE)
        bufSize = MIN_BUFFSIZE;

	do
	{
		writer = (PT_FileWriter)malloc(sizeof(T_FileWriter));
		if(!writer)
		{
			break;
		}
		
		writer->ringbuf = mallocRingBuffer(bufSize);
		if(!writer->ringbuf)
		{
			free(writer);
			writer = NULL;
			break;
		}

		writer->fd = openfile(filename,O_WRONLY);
		if(writer->fd <0)
		{
			freeRingBuffer(writer->ringbuf);
			writer->ringbuf = NULL;
			free(writer);
			writer = NULL;
			break;
		}

		ret = pthread_create(&writer->pid,NULL,do_write_thread,(void*)writer);
	    if(ret!=0)  
	    {  
			closefile(writer->fd);
			freeRingBuffer(writer->ringbuf);
			writer->ringbuf = NULL;
			free(writer);
			writer = NULL;
			break;
	    }

		pthread_mutex_init(&writer->mutex, NULL);
		pthread_cond_init(&writer->cond,NULL);
		writer->isRunning  = RUNNING;
		writer->flag	   = WRITE_SUCCESS;

	}while(0);	

	return writer;
}

int closeFileWriter(PT_FileWriter writer)
{
	if(writer)
	{
		writer->isRunning = NOT_RUNNING;
        
        while(writer->isRunning != RUNNING_QUIT)//�ȴ��˳��ɹ�
        {
            pthread_mutex_lock(&writer->mutex);
            pthread_cond_signal(&writer->cond);
            pthread_mutex_unlock(&writer->mutex);
        }

        pthread_cond_destroy(&writer->cond);
		pthread_mutex_destroy(&writer->mutex);
		closefile(writer->fd);
		freeRingBuffer(writer->ringbuf);
		writer->ringbuf = NULL;
		free(writer);
		writer = NULL;
	}

	return 0;
}

//�����ⲿ���ŵĽӿ�
int readFileWriter(PT_FileWriter writer,char *str,int size)
{
	int  ret = 0;
    int  readsize = 0;

    while(size > 0)
    {
       EB_LOGD("readFileWriter readString mWritePos=%d\r\n",writer->ringbuf->mWritePos);
       ret = readString(writer->ringbuf,str,size);
       if(ret <= 0)
       {
       	   EB_LOGD("readFileWriter wait for data\r\n");
           //����������
           
           if(!IS_RUNNING(writer))
             break;
           if(readsize)
                break;
           
           pthread_cond_wait(&writer->cond,&writer->mutex);
       }

       readsize += ret;
	   EB_LOGD("readFileWriter wait for data ret =%d\r\n",ret);
       size  -= ret;
       str  += ret;
    }

    return readsize;
}


int writeFileWriter(PT_FileWriter writer,char *str,int len)
{
    int  ret = 0,size = len;
	PT_RingBuffer ringbuf =writer->ringbuf;

    pthread_mutex_lock(&writer->mutex);
    while(size > 0)
    {
       ret = writeString(writer->ringbuf,str,size);
	   if(ret <= 0)
       {
       	  if(!IS_WRITE_SUCCESS(writer))
		  {
		  	pthread_mutex_unlock(&writer->mutex);
			return -1;
       	  }
       }
       size  -= ret;
       str  += ret;
    }
    len -= size;
    if(len > 0)
    {
        //д����� �߳̽��ж���Ȼ��д�뵽�ļ���
        pthread_cond_signal(&writer->cond);
    }
    pthread_mutex_unlock(&writer->mutex);

    return len;
}

int FileWriter_isRuning(PT_FileWriter writer)
{
    return (writer->isRunning == RUNNING);
}

