#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fileReader.h>

#define      RUNNING            1
#define      NOT_RUNNING        0
#define      RUNNING_QUIT		-1

#define   FILE_READER_DEBUG(...)
//#define   FILE_READER_DEBUG  printf

#define   isRunning(reader)	(reader->isRunning == RUNNING)
#define   isEof(reader)		(reader->flag == END_OF_FILE)

#define   IS_EMPTY(reader)  (reader->ringbuf->mReadPos == reader->ringbuf->mWritePos)


void *do_read_thread(void*arg)
{
    int  ret = 0,len;
	char tmp[1024];
	char *pbuf = NULL;
	PT_FileReader reader = (PT_FileReader)arg;
	do
	{
	    memset(tmp,0,sizeof(tmp));
        pthread_mutex_lock(&reader->mutex);
        FILE_READER_DEBUG("lock in do_read_thread\r\n");
		ret = read_fd(reader->fd,tmp,sizeof(tmp));
        if(ret <= 0)
        {
            printf("commpeter read \r\n");
            reader->flag = END_OF_FILE;
            pthread_mutex_unlock(&reader->mutex);
            break;
        }

		len  = ret;
		pbuf = tmp;
		while(len >0)
		{
			ret = writeString(reader->ringbuf,pbuf,len);
			if(ret <= 0)
			{
				//等待
				pthread_cond_wait(&reader->cond,&reader->mutex);
				usleep(20);
			}

			pbuf += ret;
			len  -= ret;
		}
 		
        pthread_mutex_unlock(&reader->mutex);
			
	}while(reader->isRunning == RUNNING);

	FILE_READER_DEBUG("exit do_read_thread isRunning= %d %d\r\n",reader->isRunning,reader->flag);
    reader->flag = END_OF_FILE;
    reader->isRunning = RUNNING_QUIT;//成功退出
	FILE_READER_DEBUG("exit do_read_thread isRunning= %d %d\r\n",reader->isRunning,reader->flag);

	return NULL;
}

PT_FileReader openFileReader(const char* filename,int bufSize)
{
	int ret;
	PT_FileReader filereader = NULL;

	do
	{
		filereader = malloc(sizeof(T_FileReader));
		if(!filereader)
		{
			break;
		}
		
		filereader->ringbuf = mallocRingBuffer(bufSize);
		if(!filereader->ringbuf)
		{
			free(filereader);
			filereader = NULL;
			break;
		}

		filereader->fd = openfile(filename,O_RDONLY);
		if(filereader->fd <0)
		{
			freeRingBuffer(filereader->ringbuf);
			filereader->ringbuf = NULL;
			free(filereader);
			filereader = NULL;
			break;
		}

		ret = pthread_create(&filereader->pid,NULL,do_read_thread,(void*)filereader);
	    if(ret!=0)  
	    {  
			closefile(filereader->fd);
			freeRingBuffer(filereader->ringbuf);
			filereader->ringbuf = NULL;
			free(filereader);
			filereader = NULL;
			break;
	    }

		pthread_mutex_init(&filereader->mutex, NULL);
		pthread_cond_init(&filereader->cond,NULL);
		filereader->isRunning  = RUNNING;
        filereader->flag = NO_END_OF_FILE;

	}while(0);

    usleep(2);

	return filereader;
}

int closeFileReader(PT_FileReader reader)
{
	if(reader)
	{
		FILE_READER_DEBUG("wait for exit %d\r\n",reader->isRunning);

		if(!isEof(reader))
		{
        	pthread_mutex_lock(&reader->mutex);
        	pthread_cond_signal(&reader->cond);
        	pthread_mutex_unlock(&reader->mutex);
		}else{
			reader->isRunning = NOT_RUNNING;
			while(reader->isRunning == RUNNING_QUIT)
	        {
	            printf(" running =%d flag = %d ",reader->isRunning,reader->flag);
	            usleep(200);
	        }//等待退出成功
		}

        //pthread_kill(tid, SIGTERM); //强制杀死
        pthread_cond_destroy(&reader->cond);
		pthread_mutex_destroy(&reader->mutex);
		closefile(reader->fd);
		freeRingBuffer(reader->ringbuf);
		reader->ringbuf = NULL;
		//free(reader);
		reader = NULL;
	}

    FILE_READER_DEBUG("closeFileReader\r\n");
	return 0;
}

int readFileReader(PT_FileReader reader,char *str,int maxsize)
{
	int  ret = 0,size = maxsize;

    FILE_READER_DEBUG("lock in readFileReader\r\n");
	pthread_mutex_lock(&reader->mutex);

    while(size > 0)
    {
       ret = readString(reader->ringbuf,str,size);
       if(ret <= 0)
       {
           FILE_READER_DEBUG("in readFileReader ret = %d running =%d flag = %d \r\n",ret,reader->isRunning,reader->flag);
		   if(isEof(reader) && (maxsize-size <= 0) )
		   {
		   		return -1;//是文件结束，并且是不在运行
		   }
		   
		   break;
       }
       size  -= ret;
       str  += ret;
    }

    
    pthread_cond_signal(&reader->cond);
    FILE_READER_DEBUG("unlock in readFileReader\r\n");
    pthread_mutex_unlock(&reader->mutex);

    return maxsize -size;
}

