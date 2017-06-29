#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <malloc.h>

#include <fileWriter.h>

#define    FILE_WRITER_DEBUG(...)

void *do_write_thread(void*arg)
{
    int  ret = 0,len;
	char tmp[1023];
	PT_FileWriter writer = (PT_FileWriter)arg;	
	printf("readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);
	do
	{
	    memset(tmp,0,sizeof(tmp));
		printf("lock readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);

        pthread_mutex_lock(&writer->mutex);
		printf("do_write_thread lock readFileWriter mWritePos=%d\r\n",writer->ringbuf->mWritePos);
        ret = readFileWriter(writer,tmp,sizeof(tmp));//������������������
		printf("do_write_thread ret =%d\r\n",ret);
        len = ret;
        ret = write_fd(writer->fd,tmp,len);
        if(ret != len)
        {
            break;
        }
        
        pthread_mutex_unlock(&writer->mutex);
		printf("do_write_thread unlock readFileWriter \r\n");	
	}while(writer->isRunning);

    writer->isRunning = -1;//�ɹ��˳�
}

PT_FileWriter openFileWriter(const char* filename,int bufSize)
{
	int ret;
	PT_FileWriter writer = NULL;

	do
	{
		writer = malloc(sizeof(PT_FileWriter));
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

		pthread_mutex_init(&writer->mutex, NULL);
		pthread_cond_init(&writer->cond,NULL);
		writer->isRunning  = 1;

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

	}while(0);	

	return writer;
}

int closeFileWriter(PT_FileWriter writer)
{
	if(writer)
	{
		writer->isRunning = 0;
        
        pthread_mutex_lock(&writer->mutex);
        pthread_cond_signal(&writer->cond);
        pthread_mutex_unlock(&writer->mutex);

        while(writer->isRunning != -1);//�ȴ��˳��ɹ�

        pthread_cond_destroy(&writer->cond);
		closefile(writer->fd);
		freeRingBuffer(writer->ringbuf);
		writer->ringbuf = NULL;
		free(writer);
		writer = NULL;
	}

	return 0;
}

//�����ⲿ���ŵĽӿ�
int readFileWriter(PT_FileWriter writer,char *str,int maxsize)
{
	int  ret = 0,size = maxsize;

    while(size > 0)
    {
       printf("readFileWriter readString mWritePos=%d\r\n",writer->ringbuf->mWritePos);
       ret = readString(writer->ringbuf,str,size);
       if(ret <= 0)
       {
       	   printf("readFileWriter wait for data\r\n");
           //����������
           pthread_cond_wait(&writer->cond,&writer->mutex);
       }
	   printf("readFileWriter wait for data ret =%d\r\n",ret);
       size  -= ret;
       str  += ret;
    }

    return maxsize - size;
}


int writeFileWriter(PT_FileWriter writer,char *str,int len)
{
    int  ret = 0,size = len;
	PT_RingBuffer ringbuf =writer->ringbuf;

	printf("write locked1 \r\n");
	printf("mWritePos =%d mReadPos =%d\r\n",ringbuf->mWritePos,ringbuf->mReadPos);
    pthread_mutex_lock(&writer->mutex);
	printf("write locked2 \r\n");
	printf("mWritePos =%d mReadPos =%d\r\n",ringbuf->mWritePos,ringbuf->mReadPos);
	printf("write locked2  str=%s  len=%d\r\n",str,len);
    while(size > 0)
    {
      
	   printf("mWritePos =%d mReadPos =%d\r\n",ringbuf->mWritePos,ringbuf->mReadPos);
       ret = writeString(writer->ringbuf,str,size);
	   printf("write str ret =%d\r\n",ret);
       if(ret <= 0)
       {
          break;
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

